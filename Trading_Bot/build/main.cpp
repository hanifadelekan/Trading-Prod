#include <iostream>
#include "fetch_hyperliquid_bbo.h"  
#include <thread>
#include <mutex>
#include <chrono>

int main() {
    // Start networking in its own thread
    std::thread net_thread([]() {
        run_bbo_async_stream("BTC", "bbo");
    });

    // Main thread can now render ImGui or do other work
    while (true) {
        {
            std::lock_guard<std::mutex> lock(bbo_mutex);
            while (!bbo_queue.empty()) {
                const auto& snapshot = bbo_queue.front();
                for (const auto& lvl : snapshot) {
                    std::cout << "Price: " << lvl.price << ", Size: " << lvl.size << ", Num Orders: " << lvl.num_orders << "\n";
                }
                std::cout << "----------------\n";
                bbo_queue.pop_front();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    net_thread.join();
    return 0;
}