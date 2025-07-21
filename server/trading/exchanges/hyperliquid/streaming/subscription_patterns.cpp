#include <iostream>
#include "core/networking/websocket_aliases.hpp"
#include <nlohmann/json.hpp>

void hyperliquid_subscribe(const std::string& symbol, const std::shared_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>>& ws, beast::error_code& ec)
    {
        nlohmann::json sub_msg = {
            {"method", "subscribe"},
            {"subscription", {{"type", "bbo"}, {"coin", symbol}}}
        };
        nlohmann::json sub_msg2 = {
            {"method", "subscribe"},
            {"subscription", {{"type", "l2Book"}, {"coin", symbol}}}
        };
        nlohmann::json sub_msg3 = {
            {"method", "subscribe"},
            {"subscription", {{"type", "trades"}, {"coin", symbol}}}
        };
        ws->write(net::buffer(sub_msg.dump()), ec);
        if (ec) { std::cerr << "BBO write subscribe error: " << ec.message() << "\n"; return; }
        ws->write(net::buffer(sub_msg2.dump()), ec);
        if (ec) { std::cerr << "l2Book write subscribe error: " << ec.message() << "\n"; return; }
        ws->write(net::buffer(sub_msg3.dump()), ec);
        if (ec) { std::cerr << "trades write subscribe error: " << ec.message() << "\n"; return; }
    }