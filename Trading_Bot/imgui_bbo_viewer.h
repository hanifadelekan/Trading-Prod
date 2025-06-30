#pragma once
#include "fetch_hyperliquid_orderbook.h" 
#include "fetch_hyperliquid_bbo.h"  // For BBOSnapshot
#include "disruptor.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <deque>       // ⬅️ Changed from <vector> to <deque>
#include <vector>

extern Disruptor<BBOSnapshot> disruptor;
extern Disruptor<OBSnapshot> obdisruptor;

class ImGuiBBOViewer {
public:
    explicit ImGuiBBOViewer(GLFWwindow* window);

    void RenderFrame();

private:
    void AddToHistory(double mid);                      // ✅ declare AddToHistory
    void AddToHistoryBBOMid(double bbo_mid); 
    void AddToHistoryImb(double imb);            // ✅ declare AddToHistoryBBOMid
    const double* midprice_history_data();               // ✅ declare midprice_history_data
    const double* bbo_midprice_history_data();
    const double* imb_history_data();
    const double* bbo_time_history_data();
    const double* imb_time_history_data();
    GLFWwindow* window_;
    uint64_t bbo_cursor_;
    uint64_t imb_cursor_;
    double last_sample_time_;
    BBOSnapshot latest_snapshot_;
    OBSnapshot latest_imb_snapshot_;

    std::deque<double> midprice_history_;
    std::deque<double> bbo_midprice_history_;
    std::deque<double> imb_history_;
    std::deque<double> imb_timestamp_history_; 
    std::deque<double> bbo_timestamp_history_;     

    
    // store timestamps in milliseconds or microseconds
    std::vector<double> imb_timestamp_buffer_; 
    std::vector<double> bbo_timestamp_buffer_;  // ImPlot wants double for time on X-axis
    std::vector<double> midprice_buffer_;
    std::vector<double> bbo_midprice_buffer_;
    std::vector<double> imb_buffer_;


    double latest_imb_ = 0.0;
    double latest_mid_ = 0.0;        // most recent midprice
    double latest_bbo_mid_ = 0.0;    // most recent weighted midprice
    double last_update_time_ = 0.0;  // last time we added a data point
    bool has_new_data_ = false;      // flag indicating whether new data arrived
// most recent weighted midprice

};
