#pragma once

#include <memory>
#include <string>
#include "hyperliquid_parser.hpp"
#include "hyperliquid_ob_parser.hpp"
#include "orderbook.hpp"
#include "disruptor.hpp"

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
);
void GuiApp::StartMarketDataStreams() ;