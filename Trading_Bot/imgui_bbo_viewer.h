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
        : window_(window), ui_cursor_(disruptor.create_consumer()) {}

    void RenderFrame();

private:
    void AddToHistory(double mid);
    const float* midprice_history_data();

    GLFWwindow* window_;
    uint64_t ui_cursor_;
    std::vector<BBOLevel> latest_snapshot_;
    std::deque<double> midprice_history_;
    std::vector<float> midprice_buffer_;
};

#endif
