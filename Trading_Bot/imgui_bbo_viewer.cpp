#include "imgui_bbo_viewer.h"
#include <implot.h>
#include <algorithm>
#include <iostream>

ImGuiBBOViewer::ImGuiBBOViewer(GLFWwindow* window)
    : window_(window), ui_cursor_(disruptor.create_consumer()), last_sample_time_(0.0),
      last_update_time_(ImGui::GetTime()), has_new_data_(false) {}

// Adds new midprice to history
void ImGuiBBOViewer::AddToHistory(double mid) {
    midprice_history_.push_back(mid);
}

// Adds new BBO-weighted midprice to history
void ImGuiBBOViewer::AddToHistoryBBOMid(double bbo_mid) {
    bbo_midprice_history_.push_back(bbo_mid);
}

// Convert midprice history deque to float buffer (with clamp)
const float* ImGuiBBOViewer::midprice_history_data() {
    const size_t MAX_HISTORY_POINTS = 5000;
    size_t history_size = midprice_history_.size();
    if (history_size > MAX_HISTORY_POINTS) {
        history_size = MAX_HISTORY_POINTS;
    }

    midprice_buffer_.resize(history_size);

    if (history_size > 0) {
        auto start_it = midprice_history_.end() - history_size;
        size_t i = 0;
        for (auto it = start_it; it != midprice_history_.end(); ++it) {
            midprice_buffer_[i++] = static_cast<float>(*it);
        }
    }
    return midprice_buffer_.data();
}

// Convert BBO-weighted midprice history deque to float buffer (with clamp)
const float* ImGuiBBOViewer::bbo_midprice_history_data() {
    const size_t MAX_HISTORY_POINTS = 5000;
    size_t history_size = bbo_midprice_history_.size();
    if (history_size > MAX_HISTORY_POINTS) {
        history_size = MAX_HISTORY_POINTS;
    }

    bbo_midprice_buffer_.resize(history_size);

    if (history_size > 0) {
        auto start_it = bbo_midprice_history_.end() - history_size;
        size_t i = 0;
        for (auto it = start_it; it != bbo_midprice_history_.end(); ++it) {
            bbo_midprice_buffer_[i++] = static_cast<float>(*it);
        }
    }
    return bbo_midprice_buffer_.data();
}

void ImGuiBBOViewer::RenderFrame() {
    try {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        // Process events
        int event_count = 0;
        const int MAX_EVENTS_PER_FRAME = 1000;
        while (event_count < MAX_EVENTS_PER_FRAME && disruptor.consume(ui_cursor_, latest_snapshot_)) {
            AddToHistory(latest_snapshot_.midprice);
            AddToHistoryBBOMid(latest_snapshot_.weighted_midprice);
            latest_mid_ = latest_snapshot_.midprice;
            latest_bbo_mid_ = latest_snapshot_.weighted_midprice;
            has_new_data_ = true;
            last_update_time_ = ImGui::GetTime();
            event_count++;
        }

        // Duplicate latest value every 50ms if no new update
        double current_time = ImGui::GetTime();
        if (current_time - last_update_time_ >= 0.05 && !has_new_data_) {
            if (!midprice_history_.empty() && !bbo_midprice_history_.empty()) {
                AddToHistory(latest_mid_);
                AddToHistoryBBOMid(latest_bbo_mid_);
                last_update_time_ = current_time;
            }
        }
        has_new_data_ = false;

        const size_t ROLLING_WINDOW = 1000;

        // Trim history to rolling window
        while (midprice_history_.size() > ROLLING_WINDOW) midprice_history_.pop_front();
        while (bbo_midprice_history_.size() > ROLLING_WINDOW) bbo_midprice_history_.pop_front();

        if (!midprice_history_.empty() && !bbo_midprice_history_.empty()) {
            const float* mid_data = midprice_history_data();
            const float* bbo_data = bbo_midprice_history_data();
            size_t mid_size = midprice_buffer_.size();
            size_t bbo_size = bbo_midprice_buffer_.size();

            if (mid_size > 0 && bbo_size > 0) {
                auto mid_minmax = std::minmax_element(midprice_buffer_.begin(), midprice_buffer_.end());
                auto bbo_minmax = std::minmax_element(bbo_midprice_buffer_.begin(), bbo_midprice_buffer_.end());

                float ymin = std::min(*mid_minmax.first, *bbo_minmax.first) * 0.9999f;
                float ymax = std::max(*mid_minmax.second, *bbo_minmax.second) * 1.0001f;

                float y_range = ymax - ymin;
                float mid_current = midprice_history_.back();
                bool need_reset = false;

                // Re-axis if latest midprice goes within 1% of y-limits
               // if (y_range > 0.0f) {
                //    float lower_dist = (mid_current - ymin) / y_range;
                //    float upper_dist = (ymax - mid_current) / y_range;
                //    if (lower_dist < 0.01f || upper_dist < 0.01f) {
                //        need_reset = true;
                 //   }
                //}

               // if (need_reset) {
                   // float mid = mid_current;
                   // ymin = mid * 0.95f;  // reset to ±5% around mid
                   // ymax = mid * 1.05f;
               // } else {
                    // Add padding for stability
                  //  float padding = y_range * 0.1f;
                  //  ymin -= padding;
                 //   ymax += padding;
                //}

                const size_t VISIBLE_XSCALE = 1500;  // leave room after rolling window

                // Get the available space inside ImGui window
                ImVec2 content_size = ImGui::GetContentRegionAvail();

                if (ImPlot::BeginPlot("Midprice & Weighted Midprice", content_size)) {
                    ImPlot::SetupAxes(nullptr, nullptr);
                    ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(VISIBLE_XSCALE), ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, ImGuiCond_Always);
                    ImPlot::PlotLine("Midprice", mid_data, mid_size);
                    ImPlot::PlotLine("WeightedMid", bbo_data, bbo_size);
                    ImPlot::EndPlot();
                }
            }
        }
    } catch (const std::length_error& e) {
        std::cerr << "Length error in RenderFrame: " << e.what() << std::endl;
        midprice_history_.clear();
        bbo_midprice_history_.clear();
    } catch (const std::exception& e) {
        std::cerr << "Exception in RenderFrame: " << e.what() << std::endl;
    }
}
