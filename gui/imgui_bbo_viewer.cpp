#include "imgui_bbo_viewer.hpp"
#include <implot.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <deque>
#include <ctime>
#include <imgui.h> // Required for ImGui::GetIO()

// ===================================================================================
//
//                              Constructor & Destructor
//
// ===================================================================================

/**
 * @brief Construct a new ImGuiBBOViewer object.
 */
ImGuiBBOViewer::ImGuiBBOViewer() {
    // Initialization is handled in the header's default member initializers.
}

// ===================================================================================
//
//                         Public Data Input Methods
//
// ===================================================================================

/**
 * @brief Public method to feed new BBO data into the viewer.
 */
void ImGuiBBOViewer::OnBBODataReceived(const BBOSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    midprice_history_.push_back(snapshot.midprice);
    bbo_midprice_history_.push_back(snapshot.weighted_midprice);
    bbo_timestamp_history_.push_back(snapshot.timestamp);
    latest_mid_ = snapshot.midprice;
    latest_bbo_mid_ = snapshot.weighted_midprice;
}

/**
 * @brief Public method to feed new order book imbalance data into the viewer.
 */
void ImGuiBBOViewer::OnImbalanceDataReceived(const OBSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    imb_history_.push_back(snapshot.imb);
    imb_timestamp_history_.push_back(snapshot.timestamp);
    latest_imb_ = snapshot.imb;
}

// ===================================================================================
//
//                                Utility Functions
//
// ===================================================================================

/**
 * @brief A custom formatter for ImPlot to display Unix timestamps as HH:MM:SS.
 */
int MyTimeFormatter(double value, char* buff, int size, void*) {
    time_t epoch_seconds = static_cast<time_t>(value);
    struct tm* local_time = localtime(&epoch_seconds);
    if (local_time) {
        return snprintf(buff, size, "%02d:%02d:%02d",
                        local_time->tm_hour,
                        local_time->tm_min,
                        local_time->tm_sec);
    }
    return snprintf(buff, size, "Invalid Time");
}

// Helper struct for the getter function used by ImPlot
struct DequeGetterData {
    const std::deque<double>& xs;
    const std::deque<double>& ys;
};

// Getter function that ImPlot will call to get data points
ImPlotPoint DequeGetter(int idx, void* user_data) {
    DequeGetterData* data = static_cast<DequeGetterData*>(user_data);
    return ImPlotPoint(data->xs[idx], data->ys[idx]);
}


// ===================================================================================
//
//                                 Main Render Frame
//
// ===================================================================================

void ImGuiBBOViewer::RenderFrame(double dir) {
    try {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        std::lock_guard<std::mutex> lock(data_mutex_);

        // --- Trim History to a Rolling Window ---
        const size_t ROLLING_WINDOW = 1000;
        while (midprice_history_.size() > ROLLING_WINDOW) midprice_history_.pop_front();
        while (bbo_midprice_history_.size() > ROLLING_WINDOW) bbo_midprice_history_.pop_front();
        while (imb_history_.size() > ROLLING_WINDOW) imb_history_.pop_front();
        while (bbo_timestamp_history_.size() > ROLLING_WINDOW) bbo_timestamp_history_.pop_front();
        while (imb_timestamp_history_.size() > ROLLING_WINDOW) imb_timestamp_history_.pop_front();

        // --- Prepare Data and Render Plot ---
        if (!midprice_history_.empty() && !bbo_midprice_history_.empty() && !imb_history_.empty()) {
            
            // --- Calculate Plot Axis Limits ---
            auto mid_minmax = std::minmax_element(midprice_history_.begin(), midprice_history_.end());
            auto bbo_minmax = std::minmax_element(bbo_midprice_history_.begin(), bbo_midprice_history_.end());
            double ymin = std::min(*mid_minmax.first, *bbo_minmax.first) * 0.9999;
            double ymax = std::max(*mid_minmax.second, *bbo_minmax.second) * 1.0001;

            double latest_timestamp = 0.0;
            if (!bbo_timestamp_history_.empty()) {
                latest_timestamp = bbo_timestamp_history_.back();
            }
            if (!imb_timestamp_history_.empty()) {
                latest_timestamp = std::max(latest_timestamp, imb_timestamp_history_.back());
            }
            
            const double VISIBLE_TIME_WINDOW_SEC = 60.0;
            double xmax_plot = latest_timestamp;
            double xmin_plot = latest_timestamp - VISIBLE_TIME_WINDOW_SEC;

            ImVec2 content_size = ImGui::GetContentRegionAvail();

            if (ImPlot::BeginPlot("Midprice & Weighted Midprice & Imb", content_size)) {
                ImPlot::SetupAxes(nullptr, nullptr);
                ImPlot::SetupAxisLimits(ImAxis_X1, xmin_plot, xmax_plot, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, ImGuiCond_Always);
                ImPlot::SetupAxisFormat(ImAxis_X1, MyTimeFormatter);

                // --- Plot directly from deques using a getter ---
                if (!bbo_timestamp_history_.empty()) {
                    DequeGetterData mid_data = {bbo_timestamp_history_, midprice_history_};
                    DequeGetterData bbo_data = {bbo_timestamp_history_, bbo_midprice_history_};
                    ImPlot::PlotLineG("Midprice", DequeGetter, &mid_data, midprice_history_.size());
                    ImPlot::PlotLineG("WeightedMid", DequeGetter, &bbo_data, bbo_midprice_history_.size());
                }
                if (!imb_timestamp_history_.empty()) {
                    DequeGetterData imb_data = {imb_timestamp_history_, imb_history_};
                    ImPlot::PlotLineG("ImbMid", DequeGetter, &imb_data, imb_history_.size());
                }

                // Draw a horizontal threshold line
                double threshold_y = dir;
                double x[2] = {xmin_plot, xmax_plot};
                double y[2] = {threshold_y, threshold_y};
                ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1));
                ImPlot::PlotLine("Threshold", x, y, 2, ImPlotLineFlags_None);

                ImPlot::EndPlot();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in RenderFrame: " << e.what() << std::endl;
    }
}
