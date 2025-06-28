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

        // Skip top bid and sum the rest
        bool first = true;
        for (const auto& [price, size] : bids) {
            if (first) { first = false; continue; }
            bid_sum += size;
        }

        // Skip top ask and sum the rest
        first = true;
        for (const auto& [price, size] : asks) {
            if (first) { first = false; continue; }
            ask_sum += size;
        }

        double total = bid_sum + ask_sum;
        if (total == 0.0) return 0.5; // neutral imbalance if empty

        return bid_sum / total;
}

};
