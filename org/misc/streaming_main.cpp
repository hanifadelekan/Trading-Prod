#include "streaming_main.hpp"
#include "ws_reader.hpp"
#include "resolve_ws.hpp"

void start_hyperliquid_stream(
    std::shared_ptr<net::io_context> io_context,
    ssl::context& ssl_ctx,
    Disruptor<BBOSnapshot>& bbo_disruptor,
    Disruptor<OBSnapshot>& ob_disruptor,
    HyperliquidParser& parser,
    HyperliquidOBParser& ob_parser,
    OrderBook& order_book,
    const std::string& symbol,
    const std::string& channel
) {
    beast::error_code ec;

    auto ws = create_secure_websocket(io_context, ssl_ctx, ec);
    if (ec || !ws) {
        std::cerr << "WebSocket creation failed: " << ec.message() << std::endl;
        return;
    }

    auto buffer = std::make_shared<beast::flat_buffer>();
    hyperliquid_subscribe(ws, symbol, ec);

    if (ec) {
        std::cerr << "Subscription failed: " << ec.message() << std::endl;
        return;
    }

    start_async_read(
        ws, buffer, io_context,
        parser, ob_parser, order_book,
        bbo_disruptor, ob_disruptor
    );
}
