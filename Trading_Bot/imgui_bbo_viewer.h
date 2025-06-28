#ifndef IMGUI_BBO_VIEWER_H
#define IMGUI_BBO_VIEWER_H

#include "disruptor.h"
#include "fetch_hyperliquid_bbo.h" // Assuming this defines BBOLevel
#include <imgui.h>
#include <GLFW/glfw3.h> // <--- ADD THIS LINE
#include <vector>
#include <deque>
#include <algorithm> // For std::min_element, std::max_element

// You no longer need these in the header since the calls were removed from constructor/destructor:
// #include <backends/imgui_impl_glfw.h>
// #include <backends/imgui_impl_opengl3.h>

extern Disruptor<std::vector<BBOLevel>> disruptor;

class ImGuiBBOViewer {
public:
    explicit ImGuiBBOViewer(GLFWwindow* window)
        : window_(window), ui_cursor_(disruptor.create_consumer()) {
        // All ImGui initialization moved to main.cpp
    }

    // Destructor: No ImGui shutdown calls here, as main.cpp handles global shutdown.
    ~ImGuiBBOViewer() {
    }

    // RenderFrame: Definition moved to .cpp.
    void RenderFrame();

private:
    void AddToHistory(double mid);
    const float* midprice_history_data();

    GLFWwindow* window_;
    uint64_t ui_cursor_;
    std::vector<BBOLevel> latest_snapshot_;
    std::deque<double> midprice_history_;
    std::vector<float> midprice_buffer_;  // for ImGui::PlotLines
};

#endif