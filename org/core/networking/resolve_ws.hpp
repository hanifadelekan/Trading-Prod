#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <memory>
#include <string>
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
);

