#include "orderbook.hpp"                    // <-- NEW: our live OrderBook class
#include "fetch_hyperliquid_orderbook.h"
#include "hyperliquid_orderbook_parser.h"
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

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;
using std::string;

HyperliquidOBParser ob_parser;
OrderBook order_book; // global/shared order book instance

const std::string HYPERLIQUID_HOST_ORDERBOOK = "api.hyperliquid.xyz";
const std::string HYPERLIQUID_PORT = "443";

void run_orderbook_async_stream(std::shared_ptr<net::io_context> io_context_ptr,
                                ssl::context& ssl_ctx,
                                const std::string& symbol) {
    auto ws = std::make_shared<websocket::stream<beast::ssl_stream<tcp::socket>>>(*io_context_ptr, ssl_ctx);
    auto buffer = std::make_shared<beast::flat_buffer>();

    beast::error_code ec;
    tcp::resolver resolver(*io_context_ptr);
    auto const results = resolver.resolve(HYPERLIQUID_HOST_ORDERBOOK, HYPERLIQUID_PORT, ec);
    if (ec) { std::cerr << "[OrderBook] Resolve error: " << ec.message() << "\n"; return; }

    net::connect(beast::get_lowest_layer(*ws), results, ec);
    if (ec) { std::cerr << "[OrderBook] Connect error: " << ec.message() << "\n"; return; }

    if (!SSL_set_tlsext_host_name(ws->next_layer().native_handle(), HYPERLIQUID_HOST_ORDERBOOK.c_str())) {
        ec.assign(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        std::cerr << "[OrderBook] Failed to set SNI hostname: " << ec.message() << "\n"; return;
    }

    ws->next_layer().set_verify_mode(ssl::verify_peer);
    ws->next_layer().handshake(ssl::stream_base::client, ec);
    if (ec) { std::cerr << "[OrderBook] SSL handshake error: " << ec.message() << "\n"; return; }

    ws->handshake(HYPERLIQUID_HOST_ORDERBOOK, "/ws", ec);
    if (ec) { std::cerr << "[OrderBook] WebSocket handshake error: " << ec.message() << "\n"; return; }

    std::cout << "[OrderBook] Connected to " << HYPERLIQUID_HOST_ORDERBOOK << "\n";

    nlohmann::json sub_msg = {
        {"method", "subscribe"},
        {"subscription", {{"type", "l2Book"}, {"coin", symbol}}}
    };
    std::cout << "[OrderBook] Sending subscription: " << sub_msg.dump() << std::endl;
    ws->write(net::buffer(sub_msg.dump()), ec);
    if (ec) { std::cerr << "[OrderBook] Subscribe write error: " << ec.message() << "\n"; return; }

    auto read_handler_ptr = std::make_shared<std::function<void(beast::error_code, std::size_t)>>();

    *read_handler_ptr = [ws, buffer, io_context_ptr, read_handler_ptr]
                        (beast::error_code ec, std::size_t) {
        if (ec == net::error::operation_aborted || ec == websocket::error::closed) {
            std::cout << "[OrderBook] WebSocket closed or operation aborted. Exiting read loop.\n"; return;
        }
        if (ec) { std::cerr << "[OrderBook] WebSocket read error: " << ec.message() << "\n"; return; }

        if (buffer->size() > 0) {
            std::string_view msg(static_cast<const char*>(buffer->data().data()), buffer->size());
    
            try {
                OrderBookSnapshot snapshot;  // ✅ declare outside so it's in scope after parsing

                auto levels_pos = msg.find("\"levels\":[");
                if (levels_pos != std::string::npos) {   // ✅ correct levels check
                    snapshot = ob_parser.parse(msg);

                    if (snapshot.updates.size() > 1000) {
                        throw std::runtime_error("[OrderBook] Abnormal message: too many levels");
                    }

                    if (!snapshot.updates.empty()) {
                        order_book.clear(); // REPLACE old book with new snapshot
                        for (const OBLevelUpdate& lvl : snapshot.updates) {
                            if (lvl.is_bid) {
                                order_book.add_bid(lvl.price, lvl.size);
                            } else {
                                order_book.add_ask(lvl.price, lvl.size);
                            }
                        }
                        double imbalance = order_book.calculate_imbalance_ignore_top();
                    
                        // order_book.print_top_levels(); // uncomment for debug
                    }
                } else {
                    std::cerr << "[OrderBook] Parser error: No levels array found\n";
                }
            } catch (const std::exception& e) {
                std::cerr << "[OrderBook] Parser error: " << e.what() << "\n";
            }
        } else {
            std::cerr << "[OrderBook] Received empty message.\n";
        }

        buffer->consume(buffer->size());
        if (!io_context_ptr->stopped()) ws->async_read(*buffer, *read_handler_ptr);
    };

    ws->async_read(*buffer, *read_handler_ptr);
}
