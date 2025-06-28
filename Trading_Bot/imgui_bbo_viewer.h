#ifndef IMGUI_BBO_VIEWER_H
#define IMGUI_BBO_VIEWER_H

#include "disruptor.h"
#include "fetch_hyperliquid_bbo.h"
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <deque>

extern Disruptor<std::vector<BBOLevel>> disruptor;

class ImGuiBBOViewer {
public:
    explicit ImGuiBBOViewer(GLFWwindow* window)
        : window_(window), ui_cursor_(disruptor.create_consumer()), last_sample_time_(0.0) {}

    void RenderFrame();

private:
    void AddToHistory(double mid);
    void AddToHistoryBBOMid(double bbo_mid);

    const float* midprice_history_data();
    const float* bbo_midprice_history_data();

    GLFWwindow* window_;
    uint64_t ui_cursor_;

    std::vector<BBOLevel> latest_snapshot_;

    std::deque<double> midprice_history_;
    std::deque<double> bbo_midprice_history_;

    std::vector<float> midprice_buffer_;
    std::vector<float> bbo_midprice_buffer_;

    double last_sample_time_;  // Time of last sampling, to drive smooth graph updates
};

#endif
