#pragma once
#include "gui/obook.hpp"
#include "gui/orderbook.hpp"
#include "gui/bbo.hpp"
#include <string>

bool parse_orderbook(const std::string& msg,
                     OrderBook& order_book,
                     Disruptor<OBSnapshot>& obdisruptor);


bool parse_trades(const std::string& msg);

bool parse_bbo(const std::string& msg,
               Disruptor<BBOSnapshot>& disruptor);

bool hl_parse(const std::string& msg,
              OrderBook& order_book,
              Disruptor<BBOSnapshot>& bbo_disruptor,
              Disruptor<OBSnapshot>& ob_disruptor);