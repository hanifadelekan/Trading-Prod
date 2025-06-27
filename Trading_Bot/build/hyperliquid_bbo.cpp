#include <vector>
#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include "fetch_hyperliquid_bbo.h"  


namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;
using std::string;

void fetch_bbo_stream(const string& symbol, const string& channel,
                      const std::function<void(const std::vector<BBOLevel>&)>& on_bbo) {
    try {
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

        beast::flat_buffer buffer;
        while (true) {
            ws.read(buffer);
            auto msg = beast::buffers_to_string(buffer.data());
            buffer.consume(buffer.size());

            json j = json::parse(msg);
            if (j.contains("data") && j["data"].contains("bbo")) {
                const auto& bbo_array = j["data"]["bbo"];
                std::vector<BBOLevel> bbo_levels;
                for (const auto& level : bbo_array) {
                    BBOLevel l;
                    l.price = std::stod(level["px"].get<std::string>());
                    l.size = std::stod(level["sz"].get<std::string>());
                    l.num_orders = level["n"].get<int>();
                    bbo_levels.push_back(l);
                }
                on_bbo(bbo_levels);  // call your handler with the new BBO snapshot
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in fetch_bbo_stream: " << e.what() << std::endl;
    }
}

