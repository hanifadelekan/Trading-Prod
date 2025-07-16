#include <iostream>
#include <atomic>
#include "gui/orderbook.hpp"
#include "gui/obook.hpp"
std::atomic<double>atomic_dir{0.0};
std::atomic<double>atomic_bbo_imbalance{0.0};
std::atomic<double>atomic_bbo_midprice{0.0};
std::atomic<double>atomic_bbo_spread{0.0};
std::atomic<double>atomic_timestamp_sec{0.0};
std::atomic<double>atomic_ob_imbalance{0.0};
std::atomic<double>atomic_ob_spread{0.0};
std::atomic<double>atomic_exposure{0.0};
inline std::shared_ptr<OrderBook> atomic_order_book_ptr;
