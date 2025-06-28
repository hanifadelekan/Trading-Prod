#include "imgui_bbo_viewer.h"
#include <iostream> // For debugging, if needed

// Implemented methods
void ImGuiBBOViewer::RenderFrame() {
    // DO NOT put glfwPollEvents(), NewFrame(), Render(), SwapBuffers() here.
    // These belong in the main application loop.

    ImGui::Begin("BBO Snapshot");
    // ImGui::GetIO().Framerate and 1000.0f / ImGui::GetIO().Framerate will be accurate
    // because ImGui::NewFrame() is called by the main loop BEFORE RenderFrame().
    ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

    // Consume BBO data if available
    // Ensure disruptor operations are thread-safe if called from RenderFrame
    // and disruptor is written to by another thread.
    // The current disruptor design with create_consumer and consume is likely already thread-safe.
    while (disruptor.consume(ui_cursor_, latest_snapshot_)) {
        if (!latest_snapshot_.empty()) {
            double bid = latest_snapshot_[0].price;
            double ask = latest_snapshot_.size() > 1 ? latest_snapshot_[1].price : bid;
            double mid = (bid + ask) / 2.0;
            AddToHistory(mid);
        }
    }

    // Display latest snapshot
    for (const auto& lvl : latest_snapshot_) {
        ImGui::Text("Price: %.2f, Size: %.4f, Orders: %d",
                    lvl.price, lvl.size, lvl.num_orders);
    }

    if (!midprice_history_.empty()) {
        // midprice_history_data() populates midprice_buffer_
        const float* plot_data = midprice_history_data(); // Get the data pointer

        float ymin = *std::min_element(midprice_buffer_.begin(), midprice_buffer_.end());
        float ymax = *std::max_element(midprice_buffer_.begin(), midprice_buffer_.end());
        
        // Add some padding to ymin/ymax to make the plot look better and avoid flat line issues
        float padding = (ymax - ymin) * 0.1f; // 10% padding
        if (padding < 0.01f) padding = 0.01f; // Minimum padding to avoid zero range
        ymin -= padding;
        ymax += padding;


        ImGui::PlotLines("Midprice History",
                        plot_data, // Use the pointer from midprice_history_data()
                        static_cast<int>(midprice_buffer_.size()),
                        0, nullptr,
                        ymin, ymax,
                        ImVec2(0, 150));
    }

    ImGui::End();

    // DO NOT call ImGui::Render() here. It belongs in main loop.
    // DO NOT call glClear(), glViewport(), ImGui_ImplOpenGL3_RenderDrawData() here. These are part of final rendering.
    // DO NOT call glfwSwapBuffers() or platform window updates here.
}

// Private helper methods (implementation)
void ImGuiBBOViewer::AddToHistory(double mid) {
    midprice_history_.push_back(mid);
    if (midprice_history_.size() > 100) { // Limit history size
        midprice_history_.pop_front();
    }
}

const float* ImGuiBBOViewer::midprice_history_data() {
    midprice_buffer_.resize(midprice_history_.size());
    for (size_t i = 0; i < midprice_history_.size(); ++i) {
        midprice_buffer_[i] = static_cast<float>(midprice_history_[i]);
    }
    return midprice_buffer_.data();
}