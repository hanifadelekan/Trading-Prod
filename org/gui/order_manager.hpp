#pragma once
#include "disruptor.h"
#include "bbo.hpp"
class OrderManager {
public:
    explicit OrderManager(Disruptor<BBOSnapshot>& disruptor)
        : disruptor_(disruptor) {}

    void init() {
        cursor_ = disruptor_.create_consumer();
    }

    void process() {
        if (disruptor_.consume(cursor_, snapshot_)) {
            if (snapshot_.levels.size() >= 2) {
                double spread = snapshot_.levels[1].price - snapshot_.levels[0].price;
                // do something with the spread
            }
        }
    }

private:
    Disruptor<BBOSnapshot>& disruptor_;
    uint64_t cursor_{0};
    BBOSnapshot snapshot_;
};
