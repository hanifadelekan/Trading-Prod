#pragma once

#include <deque>
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>  // For GLFWwindow
#include "disruptor.h"
#include "orderbook.hpp"
#include "obook.hpp"
#include "bbo.hpp"
class ImGuiBBOViewer {
public:
    ImGuiBBOViewer(GLFWwindow* window,
                   Disruptor<BBOSnapshot>& disruptor,
                   Disruptor<OBSnapshot>& obdisruptor);

    void RenderFrame(double dir);

private:
    // Update helpers
    void AddToHistory(double mid);
    void AddToHistoryBBOMid(double bbo_mid);
    void AddToHistoryImb(double imb);

    // Convert deque to fixed-size raw buffer
    const double* midprice_history_data();
    const double* bbo_midprice_history_data();
    const double* imb_history_data();
    const double* bbo_time_history_data();
    const double* imb_time_history_data();

private:
    // Disruptors
    Disruptor<BBOSnapshot>& disruptor_;
    Disruptor<OBSnapshot>& obdisruptor_;
    uint64_t bbo_cursor_;
    uint64_t imb_cursor_;

    // GUI
    GLFWwindow* window_;

    // History deques
    std::deque<double> midprice_history_;
    std::deque<double> bbo_midprice_history_;
    std::deque<double> imb_history_;
    std::deque<double> bbo_timestamp_history_;
    std::deque<double> imb_timestamp_history_;

    // Buffers for plotting
    std::vector<double> midprice_buffer_;
    std::vector<double> bbo_midprice_buffer_;
    std::vector<double> imb_buffer_;
    std::vector<double> bbo_timestamp_buffer_;
    std::vector<double> imb_timestamp_buffer_;

    // State tracking
    BBOSnapshot latest_snapshot_;
    OBSnapshot latest_imb_snapshot_;
    double last_sample_time_;
    double last_update_time_;
    bool has_new_data_;

    double latest_mid_;
    double latest_bbo_mid_;
    double latest_imb_;
};

// Timestamp formatter function for ImPlot
int MyTimeFormatter(double value, char* buff, int size, void*);
