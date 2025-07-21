#include <iostream>
#include <atomic>


extern std::atomic<double> atomic_dir;
extern std::atomic<double> atomic_bbo_imbalance;
extern std::atomic<double> atomic_bbo_midprice;
extern std::atomic<double> atomic_bbo_spread;
extern std::atomic<double> atomic_timestamp_sec;
extern std::atomic<double> atomic_ob_imbalance;
extern std::atomic<double> atomic_ob_spread;
extern std::atomic<double>atomic_exposure;
inline std::shared_ptr<OrderBook> atomic_order_book_ptr;
