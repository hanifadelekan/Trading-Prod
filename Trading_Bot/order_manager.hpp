#pragma once
#include "fetch_hyperliquid_bbo.h"
#include "disruptor.h"

class OrderManager {
public:
    void process() {
        if (disruptor.consume(cursor_, snapshot_)) {
            if (snapshot_.levels.size() >= 2) {
                double spread = snapshot_.levels[1].price - snapshot_.levels[0].price;
                // do something with the spread
            }
        }
    }

    void init() {
        cursor_ = disruptor.create_consumer();
    }

private:
    uint64_t cursor_{0};
    BBOSnapshot snapshot_;
};
