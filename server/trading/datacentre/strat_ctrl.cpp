#include <utility>  // for std::pair
#include <iostream>
#include "gui/orderbook.hpp"
#include "gui/obook.hpp"
#include "exchanges/hyperliquid/global_atomics.hpp"



std::pair<double, double> getQuotes(double weighted_midprice, double midprice) {
    double bid, ask;

    if (weighted_midprice > midprice) {
        bid = midprice - 0.01;
        ask = midprice + 0.02;
    } else if (weighted_midprice < midprice) {
        bid = midprice - 0.02;
        ask = midprice + 0.01;
    } else {
        bid = midprice - 0.01;
        ask = midprice + 0.01;
    }
    std::cout << bid << " my quotes " << ask << std::endl;
    return {bid, ask};
}

int sendOrders(std::pair<double, double> quotes){
    double bid_exposure = atomic_exposure.load();
    if (bid_exposure < 10.0){
        std::cout << "buying at" << quotes.first << std::endl;
        atomic_exposure.store(bid_exposure + 1, std::memory_order_relaxed);
        // store phantom exposure as well, and if quotes change, cancel
    }
    else{
        std::cout << "exposure limit reached" << std::endl;
    }
    return 0;
}

