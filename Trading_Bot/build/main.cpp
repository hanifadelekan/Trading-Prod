#include <iostream>
#include "fetch_hyperliquid_bbo.h"
#include <thread>
#include <chrono>
#include "disruptor.h"

extern Disruptor<std::vector<BBOLevel>> disruptor;  // use global disruptor declared in your async file

int main() {
    // Start networking in its own thread
    std::thread net_thread([]() {
        run_bbo_async_stream("BTC", "bbo");
    });

    // Create a consumer cursor initialized to the current producer sequence
    uint64_t ui_cursor = disruptor.create_consumer();

    // Main thread can now render ImGui or do other work
    while (true) {
        std::vector<BBOLevel> snapshot;
        while (disruptor.consume(ui_cursor, snapshot)) {
            for (const auto& lvl : snapshot) {
                std::cout << "Price: " << lvl.price
                          << ", Size: " << lvl.size
                          << ", Num Orders: " << lvl.num_orders << "\n";
            }
            std::cout << "----------------\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    net_thread.join();
    return 0;
}
