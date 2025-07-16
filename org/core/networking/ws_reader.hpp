#pragma once
#include "gui/obook.hpp"
#include "gui/orderbook.hpp"
#include "gui/bbo.hpp"
#include <string>
#include "websocket_aliases.hpp"
#include <boost/beast/core.hpp>  // for beast::flat_buffer
#include <boost/asio/io_context.hpp> // for net::io_context
#include "datacentre/control.hpp"
void start_async_read(
    std::shared_ptr<WebSocketType> ws,
    std::shared_ptr<beast::flat_buffer> buffer,
    std::shared_ptr<net::io_context> io_context,
    const std::string& exchange,
    OrderBook& order_book,
    Disruptor<BBOSnapshot>& bbo_disruptor,
    Disruptor<OBSnapshot>& ob_disruptor,
    HJBData bids,
    HJBData asks
);

