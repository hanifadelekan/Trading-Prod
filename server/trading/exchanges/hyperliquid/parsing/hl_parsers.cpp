#include <iostream>
#include "gui/obook.hpp"
#include "gui/orderbook.hpp"
#include "gui/bbo.hpp"
#include <string>
#include "exchanges/hyperliquid/global_atomics.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include "datacentre/strat_ctrl.hpp"
#include "datacentre/control.hpp"

bool parse_bbo(const std::string& msg,
               Disruptor<BBOSnapshot>& disruptor)
{
        BBORecord brec;
        HyperliquidParser parser;
        size_t bbo_pos = msg.find("\"bbo\":[");
        size_t arr_start = msg.find('[', bbo_pos);
        size_t arr_end   = msg.find(']', arr_start);
        if (arr_start == std::string::npos || arr_end == std::string::npos)
            throw std::runtime_error("Malformed BBO array");

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

        if (new_bbo.size() >= 2) {
            const auto& bid = new_bbo[0];
            const auto& ask = new_bbo[1];

            double mid = (bid.price + ask.price) / 2.0;
            double size_sum = bid.size + ask.size;
            double imbalance = size_sum > 0.0 ? bid.size / size_sum : 0.5;
            double weighted_mid = (imbalance * ask.price) + ((1.0 - imbalance) * bid.price);

            size_t time_pos = msg.find("\"time\":");
            std::string_view time_content(msg.data() + time_pos + 7, 13);
            double raw_timestamp_us = std::stod(std::string(time_content));
            double timestamp_sec = raw_timestamp_us / 1000.0;

            atomic_bbo_imbalance.store(weighted_mid, std::memory_order_relaxed);
            atomic_bbo_midprice.store(mid, std::memory_order_relaxed);
            atomic_bbo_spread.store(ask.price - bid.price, std::memory_order_relaxed);
            atomic_timestamp_sec.store(timestamp_sec, std::memory_order_relaxed);

            BBOSnapshot snapshot{new_bbo, mid, weighted_mid, timestamp_sec};
            disruptor.publish(snapshot);

            brec.imbalance = imbalance;
            brec.time = raw_timestamp_us;
            brec.midprice = mid;

            std::ofstream fout("/Users/hanifadelekan/dev/Trading-Prod/Trading_Bot/bbo.bin", std::ios::binary | std::ios::app);
            fout.write(reinterpret_cast<const char*>(&brec), sizeof(BBORecord));
        }

        return true;


    }


bool parse_orderbook(const std::string& msg,
                     HyperliquidOBParser& ob_parser,
                     OrderBook& order_book,
                     Disruptor<OBSnapshot>& obdisruptor)
{


        OrderBookSnapshot snapshot = ob_parser.parse(msg);

        if (snapshot.updates.size() > 1000)
            throw std::runtime_error("[OrderBook] Abnormal message: too many levels");

        if (!snapshot.updates.empty()) {
            order_book.clear();  // full snapshot
            for (const auto& lvl : snapshot.updates) {
                if (lvl.is_bid) order_book.add_bid(lvl.price, lvl.size);
                else            order_book.add_ask(lvl.price, lvl.size);
            }
            std::shared_ptr<OrderBook> snapshot = std::make_shared<OrderBook>(order_book);

            atomic_order_book_ptr = snapshot;

            auto imbalance_data = order_book.calculate_imbalance();

            double imbalance = imbalance_data[0];
            double ob_spread = imbalance_data[1];

            size_t time_pos = msg.find("\"time\":");
            std::string_view time_content(msg.data() + time_pos + 7, 13);
            double raw_timestamp_us = std::stod(std::string(time_content));
            double timestamp_sec = raw_timestamp_us / 1000.0;

            atomic_ob_imbalance.store(imbalance, std::memory_order_relaxed);
            atomic_ob_spread.store(ob_spread, std::memory_order_relaxed);

            OBSnapshot ob_snapshot{imbalance, timestamp_sec};
            obdisruptor.publish(ob_snapshot);

            OBRecord obrec;
            obrec.imbalance = imbalance_data[0];
            obrec.time = raw_timestamp_us;
            obrec.bid1 = imbalance_data[1];
            obrec.bid2 = imbalance_data[2];
            obrec.ask1 = imbalance_data[3];
            obrec.ask2 = imbalance_data[4];
            double bid = imbalance_data[1];
            double ask = imbalance_data[2];
            double midprice = ( bid + ask)/2;
            imbalance = ((imbalance * bid)+((1 - imbalance)* ask));
            std::pair<double,double> quotes = getQuotes(imbalance,midprice);
            sendOrders(quotes);

            std::ofstream fout("/Users/hanifadelekan/dev/Trading-Prod/Trading_Bot/ob.bin", std::ios::binary | std::ios::app);
            fout.write(reinterpret_cast<const char*>(&obrec), sizeof(OBRecord));
        }

        return true;

    
}


bool parse_trades(const std::string& msg) {


        nlohmann::json jmsg = nlohmann::json::parse(msg);

        for (const auto& trade : jmsg["data"]) {
            TradeRecord rec;

            std::string price = trade.at("px").get<std::string>();
            std::string size  = trade.at("sz").get<std::string>();
            std::string side  = trade.at("side").get<std::string>();
            std::string time  = std::to_string(trade.at("time").get<double>());

            strncpy(rec.price, price.c_str(), sizeof(rec.price));
            strncpy(rec.size,  size.c_str(),  sizeof(rec.size));
            strncpy(rec.side,  side.c_str(),  sizeof(rec.side));
            strncpy(rec.time,  time.c_str(),  sizeof(rec.time));

            std::ofstream fout("/Users/hanifadelekan/dev/Trading-Prod/Trading_Bot/trades.bin", std::ios::binary | std::ios::app);
            fout.write(reinterpret_cast<const char*>(&rec), sizeof(TradeRecord));
        }

        return true;

 
}


bool hl_parse(const std::string& msg,
              OrderBook& order_book,
              Disruptor<BBOSnapshot>& bbo_disruptor,
              Disruptor<OBSnapshot>& ob_disruptor,
              HJBData bids,
              HJBData asks)
{
    bool parsed = false;
    HyperliquidOBParser ob_parser;
    try {
        // Parse BBO
        if (msg.find("\"bbo\":[") != std::string::npos) {
            parsed |= parse_bbo(
                msg,
                bbo_disruptor
            );
        }

        // Parse OrderBook
        if (msg.find("\"levels\":[") != std::string::npos) {
            parsed |= parse_orderbook(
                msg, ob_parser, order_book,
                ob_disruptor
            );
        }

        // Parse Trades
        if (msg.find("\"trades\"") != std::string::npos) {
            parsed |= parse_trades(msg);
        }
        double z = atomic_bbo_imbalance.load();
        double midprice = atomic_bbo_midprice.load();
        z = (z - midprice) * 10;
        double q = 4;
        double vol = 0.01;
        double bid = phi_at(bids,q,z,vol);
        double ask = phi_at(asks,q,z,vol);
        std::cout << "ask : "<< midprice - bid << "\n" << "midprice : " << midprice << "\n" << "bid : " << midprice + ask << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[HL Parse] General error: " << e.what() << "\n";
        return false;
    }


    return parsed;
}
