#ifndef IMGUI_BBO_VIEWER_HPP
#define IMGUI_BBO_VIEWER_HPP

#include <deque>
#include <vector>
#include <mutex>
#include <nlohmann/json.hpp> // Include JSON header

// Forward-declare GLFWwindow to avoid including the GLFW header here.
struct GLFWwindow;

class ImGuiBBOViewer {
public:
    // --- Constructor & Destructor ---
    ImGuiBBOViewer();
    ~ImGuiBBOViewer() = default;

    // --- Public Data Input Methods ---
    // This single method now accepts a json object directly.
    void OnDataReceived(const nlohmann::json& message);

    // --- Main Render Method ---
    void RenderFrame(double dir);

private:
    // --- Thread Safety ---
    std::mutex data_mutex_; // Mutex to protect access to the deques

    // --- Data Storage (History) ---
    std::deque<double> midprice_history_;
    std::deque<double> bbo_midprice_history_;
    std::deque<double> imb_history_;
    std::deque<double> bbo_timestamp_history_;
    std::deque<double> imb_timestamp_history_;

    // --- Latest Data Cache ---
    double latest_mid_ = 0.0;
    double latest_bbo_mid_ = 0.0;
    double latest_imb_ = 0.0;
};

#endif // IMGUI_BBO_VIEWER_HPP
