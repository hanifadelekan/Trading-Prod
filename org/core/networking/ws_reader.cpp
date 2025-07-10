#include "exchanges/hyperliquid/parsing/hl_parsers.hpp"
#include "ws_reader.hpp"
#include "gui/obook.hpp"
#include "gui/orderbook.hpp"
#include "gui/bbo.hpp"
#include <string>
#include "websocket_aliases.hpp"
#include <boost/beast/core.hpp>  // for beast::flat_buffer
#include <boost/asio/io_context.hpp> // for net::io_context


void start_async_read(
    std::shared_ptr<WebSocketType> ws,
    std::shared_ptr<beast::flat_buffer> buffer,
    std::shared_ptr<net::io_context> io_context,
    const std::string& exchange,
    OrderBook& order_book,
    Disruptor<BBOSnapshot>& bbo_disruptor,
    Disruptor<OBSnapshot>& ob_disruptor
) {
    ws->async_read(
        *buffer,
        [ws, buffer, io_context, exchange,
         &order_book, &bbo_disruptor, &ob_disruptor]
        (beast::error_code ec, std::size_t bytes_transferred) mutable {

            if (ec) {
                std::cerr << "Read error: " << ec.message() << "\n";
                return;
            }

            std::string msg = beast::buffers_to_string(buffer->data());

            if (exchange == "hyperliquid") {
                hl_parse(msg, order_book, bbo_disruptor, ob_disruptor);

            }

            // TODO: Add handling for "binance", "okx", etc.

            buffer->consume(buffer->size());

            // Recursively continue
            start_async_read(
                ws, buffer, io_context, exchange,
                order_book, bbo_disruptor, ob_disruptor
            );
        }
    );
}
