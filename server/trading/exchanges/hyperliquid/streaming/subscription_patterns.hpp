#pragma once


#include <iostream>
#include "core/networking/websocket_aliases.hpp"
#include <nlohmann/json.hpp>



void hyperliquid_subscribe(const std::string& symbol, const std::shared_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>>& ws
,beast::error_code& ec);
