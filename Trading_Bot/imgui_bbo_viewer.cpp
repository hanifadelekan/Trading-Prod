#include "imgui_bbo_viewer.h"
#include <implot.h>
#include <iostream>

void ImGuiBBOViewer::RenderFrame() {
    ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

    // Consume BBO data if available
    while (disruptor.consume(ui_cursor_, latest_snapshot_)) {
        if (latest_snapshot_.size() >= 2) {
            const auto& bid_level = latest_snapshot_[0];
            const auto& ask_level = latest_snapshot_[1];

            double bid = bid_level.price;
            double ask = ask_level.price;
            double bid_size = bid_level.size;
            double ask_size = ask_level.size;

            double mid = (bid + ask) / 2.0;
            double size_sum = bid_size + ask_size;
            double bbo_imbalance = size_sum > 0.0 ? bid_size / size_sum : 0.5;
            double bbo_weighted_mid = (bbo_imbalance * ask) + ((1.0 - bbo_imbalance) * bid);

            AddToHistory(mid);
            AddToHistoryBBOMid(bbo_weighted_mid);
        }
    }

    // Continuous sampling: add latest value every fixed interval to smooth the graph
    double now = ImGui::GetTime();  // seconds since ImGui init
    const double interval = 0.05;   // 50 ms sample rate
    if (now - last_sample_time_ >= interval) {
        last_sample_time_ = now;

        if (!midprice_history_.empty()) {
            AddToHistory(midprice_history_.back());
        }
        if (!bbo_midprice_history_.empty()) {
            AddToHistoryBBOMid(bbo_midprice_history_.back());
        }
    }

    // Display latest snapshot
    for (const auto& lvl : latest_snapshot_) {
        ImGui::Text("Price: %.2f, Size: %.4f, Orders: %d", lvl.price, lvl.size, lvl.num_orders);
    }

    // Plot both series using ImPlot
    if (!midprice_history_.empty() && !bbo_midprice_history_.empty()) {
        const float* mid_data = midprice_history_data();
        const float* bbo_data = bbo_midprice_history_data();

        auto mid_minmax = std::minmax_element(midprice_buffer_.begin(), midprice_buffer_.end());
        auto bbo_minmax = std::minmax_element(bbo_midprice_buffer_.begin(), bbo_midprice_buffer_.end());
        float ymin = std::min(*mid_minmax.first, *bbo_minmax.first);
        float ymax = std::max(*mid_minmax.second, *bbo_minmax.second);
        float padding = (ymax - ymin) * 0.5f;
        if (padding < 0.05f) padding = 0.05f;
        ymin -= padding;
        ymax += padding;

        if (ImPlot::BeginPlot("Midprice & BBO-weighted Midprice History", ImVec2(-1, 300))) {
            ImPlot::SetupAxes(nullptr, nullptr);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, midprice_buffer_.size(), ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, ImGuiCond_Always);

            ImPlot::PlotLine("Midprice", mid_data, midprice_buffer_.size());
            ImPlot::PlotLine("BBO-weighted Midprice", bbo_data, bbo_midprice_buffer_.size());

            ImPlot::EndPlot();
        }
    }
}

void ImGuiBBOViewer::AddToHistory(double mid) {
    midprice_history_.push_back(mid);
    if (midprice_history_.size() > 100) {
        midprice_history_.pop_front();
    }
}

void ImGuiBBOViewer::AddToHistoryBBOMid(double BBOmid) {
    bbo_midprice_history_.push_back(BBOmid);
    if (bbo_midprice_history_.size() > 100) {
        bbo_midprice_history_.pop_front();
    }
}

const float* ImGuiBBOViewer::midprice_history_data() {
    midprice_buffer_.resize(midprice_history_.size());
    for (size_t i = 0; i < midprice_history_.size(); ++i) {
        midprice_buffer_[i] = static_cast<float>(midprice_history_[i]);
    }
    return midprice_buffer_.data();
}

const float* ImGuiBBOViewer::bbo_midprice_history_data() {
    bbo_midprice_buffer_.resize(bbo_midprice_history_.size());
    for (size_t i = 0; i < bbo_midprice_history_.size(); ++i) {
        bbo_midprice_buffer_[i] = static_cast<float>(bbo_midprice_history_[i]);
    }
    return bbo_midprice_buffer_.data();
}
