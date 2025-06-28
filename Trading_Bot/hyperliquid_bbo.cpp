#include "fetch_hyperliquid_bbo.h"
#include "hyperliquid_parser.h"  // <-- your new fast parser header
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <iostream>
#include <thread>
#include "disruptor.h"
#include <chrono>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;
using std::string;

// Global Disruptor instance (single producer)
Disruptor<std::vector<BBOLevel>> disruptor(1024);
HyperliquidParser parser;  // your custom parser instance

void run_bbo_async_stream(const string& symbol, const string& channel) {
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

    if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), "api.hyperliquid.xyz")) {
        throw beast::system_error(
            beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()),
            "Failed to set SNI Hostname"
        );
    }
    ws.next_layer().set_verify_mode(ssl::verify_peer);
    ws.next_layer().handshake(ssl::stream_base::client);
    ws.handshake("api.hyperliquid.xyz", "/ws");

    // Subscribe to Hyperliquid BBO
    nlohmann::json sub_msg = {
        {"method", "subscribe"},
        {"subscription", {{"type", channel}, {"coin", symbol}}}
    };
    ws.write(net::buffer(sub_msg.dump()));

    auto buffer = std::make_shared<beast::flat_buffer>();

    std::function<void(beast::error_code, std::size_t)> read_handler;

    read_handler = [&](beast::error_code ec, std::size_t) {
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
        auto start = std::chrono::high_resolution_clock::now();

        // Manually locate start of "bbo":[
        size_t bbo_array_pos = msg.find("\"bbo\":[");
        if (bbo_array_pos == std::string::npos) {
            throw std::runtime_error("No 'bbo' array found in message!");
        }

        size_t arr_start = msg.find('[', bbo_array_pos);
        size_t arr_end   = msg.find(']', arr_start);
        if (arr_start == std::string::npos || arr_end == std::string::npos || arr_end <= arr_start) {
            throw std::runtime_error("Malformed BBO array!");
        }

        std::string_view array_content(msg.data() + arr_start + 1, arr_end - arr_start - 1);

        std::vector<BBOLevel> new_bbo;

        size_t pos = 0;
        while (pos < array_content.size()) {
            size_t obj_start = array_content.find('{', pos);
            if (obj_start == std::string::npos) break;

            size_t obj_end = array_content.find('}', obj_start);
            if (obj_end == std::string::npos) break;

            std::string_view level_json(array_content.data() + obj_start, obj_end - obj_start + 1);

            BBOLevel lvl = parser.parseLevel(level_json);
            new_bbo.push_back(lvl);

            pos = obj_end + 1;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duration_us = end - start;

        std::cout << "[Parser] Custom parse took " << duration_us.count() << " us\n";

        disruptor.publish(new_bbo);
    } catch (const std::exception& e) {
        std::cerr << "Custom parser error: " << e.what() << "\n";
    }


        ws.async_read(*buffer, read_handler);
    };

    ws.async_read(*buffer, read_handler);

    auto work = net::make_work_guard(io_context);
    io_context.run();
}
