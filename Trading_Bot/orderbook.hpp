#pragma once

#include <map>
#include <iostream>

class OrderBook {
public:
    std::map<double, double, std::greater<>> bids; // price → size, sorted descending
    std::map<double, double> asks;                 // price → size, sorted ascending

    void clear() {
        bids.clear();
        asks.clear();
    }

    void add_bid(double price, double size) {
        bids[price] = size;
    }

    void add_ask(double price, double size) {
        asks[price] = size;
    }

    void print_top_levels(size_t depth = 5) const {
        std::cout << "[OrderBook] Top " << depth << " bids:\n";
        size_t count = 0;
        for (const auto& [price, size] : bids) {
            std::cout << "  B " << price << "@" << size << "\n";
            if (++count >= depth) break;
        }
        count = 0;
        std::cout << "[OrderBook] Top " << depth << " asks:\n";
        for (const auto& [price, size] : asks) {
            std::cout << "  A " << price << "@" << size << "\n";
            if (++count >= depth) break;
        }
    }
    double calculate_imbalance_ignore_top() const {
    double bid_sum = 0.0, ask_sum = 0.0;
    double second_bid_price = 0.0, second_ask_price = 0.0;

    // Skip top bid; track second best bid price
    bool first = true;
    size_t bid_level = 0;
    for (const auto& [price, size] : bids) {
        if (first) { first = false; continue; }
        if (bid_level == 0) second_bid_price = price; // first bid after skipping top
        bid_sum += size;
        ++bid_level;
    }

    // Skip top ask; track second best ask price
    first = true;
    size_t ask_level = 0;
    for (const auto& [price, size] : asks) {
        if (first) { first = false; continue; }
        if (ask_level == 0) second_ask_price = price; // first ask after skipping top
        ask_sum += size;
        ++ask_level;
    }

    double total = bid_sum + ask_sum;
    if (total == 0.0 || second_ask_price == 0.0 || second_bid_price == 0.0) {
        return 0.0; // fallback: unable to compute
    }

    double imbalance = bid_sum / total;
    return imbalance * second_ask_price + (1.0 - imbalance) * second_bid_price;
}


};
