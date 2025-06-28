#pragma once

#include "fetch_hyperliquid_bbo.h"  // For BBOSnapshot
#include "disruptor.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <deque>       // ⬅️ Changed from <vector> to <deque>
#include <vector>

extern Disruptor<BBOSnapshot> disruptor;

class ImGuiBBOViewer {
public:
    explicit ImGuiBBOViewer(GLFWwindow* window);

    void RenderFrame();

private:
    void AddToHistory(double mid);                      // ✅ declare AddToHistory
    void AddToHistoryBBOMid(double bbo_mid);            // ✅ declare AddToHistoryBBOMid
    const float* midprice_history_data();               // ✅ declare midprice_history_data
    const float* bbo_midprice_history_data();
    
    GLFWwindow* window_;
    uint64_t ui_cursor_;
    double last_sample_time_;
    BBOSnapshot latest_snapshot_;

    std::deque<double> midprice_history_;
    std::deque<double> bbo_midprice_history_;

    std::vector<float> midprice_buffer_;
    std::vector<float> bbo_midprice_buffer_;

    double latest_mid_ = 0.0;        // most recent midprice
    double latest_bbo_mid_ = 0.0;    // most recent weighted midprice
    double last_update_time_ = 0.0;  // last time we added a data point
    bool has_new_data_ = false;      // flag indicating whether new data arrived
// most recent weighted midprice

};
