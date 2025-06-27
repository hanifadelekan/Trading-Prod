#include "fetch_hyperliquid_bbo.h"
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;
using json = nlohmann::json;
using std::string;
#include <queue>
#include <deque>

std::deque<std::vector<BBOLevel>> bbo_queue;


std::mutex bbo_mutex;


void run_bbo_async_stream(const string& symbol, const string& channel) {
    // Dedicated io_context for network thread
    net::io_context io_context;
    ssl::context ssl_ctx{ssl::context::tlsv12_client};
    ssl_ctx.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::no_sslv3 |
        ssl::context::no_tlsv1 |
        ssl::context::no_tlsv1_1 |
        ssl::context::single_dh_use
    );
    ssl_ctx.set_default_verify_paths();

    websocket::stream<beast::ssl_stream<tcp::socket>> ws(io_context, ssl_ctx);

    tcp::resolver resolver(io_context);
    auto const results = resolver.resolve("api.hyperliquid.xyz", "443");
    net::connect(ws.next_layer().next_layer(), results);

    if(!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), "api.hyperliquid.xyz")) {
        throw beast::system_error(
            beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()),
            "Failed to set SNI Hostname"
        );
    }
    ws.next_layer().set_verify_mode(ssl::verify_peer);
    ws.next_layer().handshake(ssl::stream_base::client);
    ws.handshake("api.hyperliquid.xyz", "/ws");

    json sub_msg = {
        {"method", "subscribe"},
        {"subscription", {{"type", channel}, {"coin", symbol}}}
    };
    ws.write(net::buffer(sub_msg.dump()));

    auto buffer = std::make_shared<beast::flat_buffer>();

    // Start async read loop
    std::function<void(beast::error_code, std::size_t)> read_handler;

   read_handler = [&](beast::error_code ec, std::size_t bytes_transferred) {
    if (ec) {
        if (ec == beast::websocket::error::closed || ec == boost::asio::error::eof) {
            std::cerr << "WebSocket closed.\n";
        } else {
            std::cerr << "WebSocket error: " << ec.message() << "\n";
        }
        return;
    }

    std::string msg = beast::buffers_to_string(buffer->data());
    buffer->consume(buffer->size());

    try {
        json j = json::parse(msg);
        if (j.contains("data") && j["data"].contains("bbo")) {
            std::vector<BBOLevel> new_bbo;
            for (const auto& level : j["data"]["bbo"]) {
                BBOLevel l;
                l.price = std::stod(level["px"].get<std::string>());
                l.size = std::stod(level["sz"].get<std::string>());
                l.num_orders = level["n"].get<int>();
                new_bbo.push_back(l);
            }

                        // Safely queue the new snapshot
            {
                std::lock_guard<std::mutex> lock(bbo_mutex);
                if (bbo_queue.size() >= 1000) {
                    // Remove oldest snapshot to make room
                    bbo_queue.pop_front();
                }
                bbo_queue.push_back(new_bbo);
            }

        }
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
    }

    ws.async_read(*buffer, read_handler);  // re-register
};


    // Start first async read
    ws.async_read(*buffer, read_handler);

    // Prevent io_context from exiting early
    auto work = net::make_work_guard(io_context);

    // Run the io_context loop in this thread
    io_context.run();
}