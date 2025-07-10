#pragma once
#include <map>
#include <iostream>
#include <atomic>
#include <array>

class OrderBook {
public:
    std::map<double, double, std::greater<>> bids; // price → size, sorted descending
    std::map<double, double> asks;                 // price → size, sorted ascending

    // ✅ Add copy constructor
    OrderBook(const OrderBook& other)
        : bids(other.bids), asks(other.asks) {}

    OrderBook() = default;

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

    std::array<double, 5> calculate_imbalance() const {
        double bid_sum = 0.0, ask_sum = 0.0;
        double best_bid = 0.0, second_bid = 0.0;
        double best_ask = 0.0, second_ask = 0.0;

        // Bids: get top 2 bids & sum levels 2-20
        size_t bid_level = 0;
        for (const auto& [price, size] : bids) {
            ++bid_level;
            if (bid_level == 1) best_bid = price;
            else if (bid_level == 2) second_bid = price;

            if (bid_level > 20) break;
            if (bid_level >= 2) bid_sum += size;
        }

        // Asks: get top 2 asks & sum levels 2-20
        size_t ask_level = 0;
        for (const auto& [price, size] : asks) {
            ++ask_level;
            if (ask_level == 1) best_ask = price;
            else if (ask_level == 2) second_ask = price;

            if (ask_level > 20) break;
            if (ask_level >= 2) ask_sum += size;
        }

        double total = bid_sum + ask_sum;
        if (total == 0.0) {
            return {0.0, 0.0, 0.0, 0.0, 0.0};
        }

        double imbalance = bid_sum / total;
        return {imbalance, best_bid, second_bid, best_ask, second_ask};
    }
};
