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

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;
using std::string;

struct BBOLevel {
    double price;
    double size;
    int num_orders;
};

std::vector<BBOLevel> fetch_latest_bbo(const string& symbol, const string& channel) {
    std::vector<BBOLevel> bbo_levels;

    try {
        net::io_context io_context;
        ssl::context ssl_ctx{ssl::context::tlsv12_client};

        // Secure SSL configuration
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

        // Resolve DNS
        tcp::resolver resolver(io_context);
        auto const results = resolver.resolve("api.hyperliquid.xyz", "443");

        // Connect TCP
        net::connect(ws.next_layer().next_layer(), results);

        // Set SNI
        if(!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), "api.hyperliquid.xyz")) {
            throw beast::system_error(
                beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()),
                "Failed to set SNI Hostname"
            );
        }

        ws.next_layer().set_verify_mode(ssl::verify_peer);

        // TLS handshake
        ws.next_layer().handshake(ssl::stream_base::client);

        // WebSocket handshake
        ws.handshake("api.hyperliquid.xyz", "/ws");

        // Send subscription message
        json sub_msg = {
            {"method", "subscribe"},
            {"subscription", {
                {"type", channel},
                {"coin", symbol}
            }}
        };
        ws.write(net::buffer(sub_msg.dump()));

        // Read a single message (blocking until received)
        beast::flat_buffer buffer;
        ws.read(buffer);
        auto msg = beast::buffers_to_string(buffer.data());

        // Parse JSON into vector
        json j = json::parse(msg);

        if (j.contains("data") && j["data"].contains("bbo")) {
            const auto& bbo_array = j["data"]["bbo"];
            for (const auto& level : bbo_array) {
                BBOLevel l;
                l.price = std::stod(level["px"].get<std::string>());
                l.size = std::stod(level["sz"].get<std::string>());
                l.num_orders = level["n"].get<int>();
                bbo_levels.push_back(l);
            }
        }

        ws.close(websocket::close_code::normal); // Clean close
    }
    catch (const std::exception& e) {
        std::cerr << "Error in fetch_latest_bbo: " << e.what() << std::endl;
    }

    return bbo_levels;
}
