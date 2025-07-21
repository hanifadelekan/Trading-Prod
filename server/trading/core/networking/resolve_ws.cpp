

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <memory>
#include <string>
#include "resolve_ws.hpp"
#include <iostream>
#include "websocket_aliases.hpp"


std::tuple<
    std::shared_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>>,
    std::shared_ptr<beast::flat_buffer>
>
start_websocket_stream(
    const std::string& host,
    const std::string& port,
    const std::string& target,
    std::shared_ptr<net::io_context> io_context,
    ssl::context& ssl_ctx
)
 {
    beast::error_code ec;

    auto ws = std::make_shared<websocket::stream<beast::ssl_stream<tcp::socket>>>(*io_context, ssl_ctx);

    tcp::resolver resolver(*io_context);
    auto results = resolver.resolve(host, port, ec);
    if (ec) {
        std::cerr << "Resolve failed: " << ec.message() << "\n";
        return {nullptr, nullptr};
    }

    net::connect(beast::get_lowest_layer(*ws), results, ec);
    if (ec) {
        std::cerr << "Connect failed: " << ec.message() << "\n";
        return {nullptr, nullptr};
    }

    if (!SSL_set_tlsext_host_name(ws->next_layer().native_handle(), host.c_str())) {
        ec.assign(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        std::cerr << "SNI error: " << ec.message() << "\n";
        return {nullptr, nullptr};
    }

    ws->next_layer().set_verify_mode(ssl::verify_peer);
    ws->next_layer().handshake(ssl::stream_base::client, ec);
    if (ec) {
        std::cerr << "SSL handshake failed: " << ec.message() << "\n";
        return {nullptr, nullptr};
    }

    ws->handshake(host, target, ec);
    if (ec) {
        std::cerr << "WebSocket handshake failed: " << ec.message() << "\n";
        return {nullptr, nullptr};
    }

    auto buffer = std::make_shared<beast::flat_buffer>();
    return {ws, buffer};
}
