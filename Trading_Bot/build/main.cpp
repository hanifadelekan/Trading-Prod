#include <iostream>
#include "fetch_hyperliquid_bbo.h"  


int main() {
    fetch_bbo_stream("BTC", "bbo", [](const std::vector<BBOLevel>& levels) {
        for (const auto& lvl : levels) {
            std::cout << "Price: " << lvl.price
                      << ", Size: " << lvl.size
                      << ", Num Orders: " << lvl.num_orders << "\n";
        }
        std::cout << "---------------------\n";
    });
    return 0;
}


