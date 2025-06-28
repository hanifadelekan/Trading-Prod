#include "fetch_hyperliquid_bbo.h"
#include "hyperliquid_parser.h"
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <iostream>
#include <chrono>
#include <nlohmann/json.hpp>
#include <functional>
#include <memory>
#include "disruptor.h"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;
using std::string;


extern HyperliquidParser parser;

const std::string HYPERLIQUID_HOST_BBO = "api.hyperliquid.xyz";
const std::string HYPERLIQUID_PORT = "443";

void run_bbo_async_stream(std::shared_ptr<net::io_context> io_context_ptr,
                          ssl::context& ssl_ctx,
                          const string& symbol, const string& channel) {
    auto ws = std::make_shared<websocket::stream<beast::ssl_stream<tcp::socket>>>(*io_context_ptr, ssl_ctx);
    auto buffer = std::make_shared<beast::flat_buffer>();

    beast::error_code ec;
    tcp::resolver resolver(*io_context_ptr);
    auto const results = resolver.resolve(HYPERLIQUID_HOST_BBO, HYPERLIQUID_PORT, ec);
    if (ec) { std::cerr << "BBO resolve error: " << ec.message() << "\n"; return; }

    net::connect(beast::get_lowest_layer(*ws), results, ec);
    if (ec) { std::cerr << "BBO connect error: " << ec.message() << "\n"; return; }

    if (!SSL_set_tlsext_host_name(ws->next_layer().native_handle(), HYPERLIQUID_HOST_BBO.c_str())) {
        ec.assign(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        std::cerr << "Failed to set SNI Hostname: " << ec.message() << "\n"; return;
    }

    ws->next_layer().set_verify_mode(ssl::verify_peer);
    ws->next_layer().handshake(ssl::stream_base::client, ec);
    if (ec) { std::cerr << "BBO SSL handshake error: " << ec.message() << "\n"; return; }

    ws->handshake(HYPERLIQUID_HOST_BBO, "/ws", ec);
    if (ec) { std::cerr << "BBO WebSocket handshake error: " << ec.message() << "\n"; return; }

    std::cout << "BBO WebSocket connected to " << HYPERLIQUID_HOST_BBO << "\n";

    nlohmann::json sub_msg = {
        {"method", "subscribe"},
        {"subscription", {{"type", channel}, {"coin", symbol}}}
    };
    ws->write(net::buffer(sub_msg.dump()), ec);
    if (ec) { std::cerr << "BBO write subscribe error: " << ec.message() << "\n"; return; }

    auto read_handler_ptr = std::make_shared<std::function<void(beast::error_code, std::size_t)>>();

    *read_handler_ptr = [ws, buffer, io_context_ptr, read_handler_ptr]
                        (beast::error_code ec, std::size_t) {
        if (ec == net::error::operation_aborted || ec == websocket::error::closed) {
            std::cout << "BBO WebSocket read operation aborted or closed. Exiting read loop.\n"; return;
        }
        if (ec) { std::cerr << "BBO WebSocket read error: " << ec.message() << "\n"; return; }

        std::string msg = beast::buffers_to_string(buffer->data());
        buffer->consume(buffer->size());

        try {
            size_t bbo_pos = msg.find("\"bbo\":[");
            if (bbo_pos != std::string::npos) {
                size_t arr_start = msg.find('[', bbo_pos);
                size_t arr_end   = msg.find(']', arr_start);
                if (arr_start == std::string::npos || arr_end == std::string::npos) throw std::runtime_error("Malformed BBO array");
                std::string_view array_content(msg.data() + arr_start + 1, arr_end - arr_start - 1);

                std::vector<BBOLevel> new_bbo;
                size_t pos = 0;
                while (pos < array_content.size()) {
                    size_t obj_start = array_content.find('{', pos);
                    if (obj_start == std::string::npos) break;
                    size_t obj_end = array_content.find('}', obj_start);
                    if (obj_end == std::string::npos) break;
                    std::string_view level_json(array_content.data() + obj_start, obj_end - obj_start + 1);
                    new_bbo.push_back(parser.parseLevel(level_json));
                    pos = obj_end + 1;
                }
                if (!new_bbo.empty()) {
                    double mid = 0.0, weighted_mid = 0.0;
                    if (new_bbo.size() >= 2) {
                        const auto& bid = new_bbo[0];
                        const auto& ask = new_bbo[1];
                        mid = (bid.price + ask.price) / 2.0;
                        double size_sum = bid.size + ask.size;
                        double imbalance = size_sum > 0.0 ? bid.size / size_sum : 0.5;
                        weighted_mid = (imbalance * ask.price) + ((1.0 - imbalance) * bid.price);
                    }
                    BBOSnapshot snapshot{new_bbo, mid, weighted_mid};
                    disruptor.publish(snapshot);
}
            }
        } catch (const std::exception& e) {
            std::cerr << "BBO parser error: " << e.what() << "\n";
        }

        if (!io_context_ptr->stopped()) ws->async_read(*buffer, *read_handler_ptr);
    };

    ws->async_read(*buffer, *read_handler_ptr);
}
