// core/networking/websocket_aliases.hpp
#pragma once
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = net::ip::tcp;

using WebSocketType = websocket::stream<beast::ssl_stream<tcp::socket>>;
