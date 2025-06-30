#include "imgui_bbo_viewer.h"
#include <implot.h>
#include <algorithm>
#include <iostream>

ImGuiBBOViewer::ImGuiBBOViewer(GLFWwindow* window)
    : window_(window),
      bbo_cursor_(disruptor.create_consumer()),
      imb_cursor_(obdisruptor.create_consumer()),
      last_sample_time_(0.0),
      last_update_time_(ImGui::GetTime()),
      has_new_data_(false) {}

// Adds new midprice to history
void ImGuiBBOViewer::AddToHistory(double mid) {
    midprice_history_.push_back(mid);
}

// Adds new BBO-weighted midprice to history
void ImGuiBBOViewer::AddToHistoryBBOMid(double bbo_mid) {
    bbo_midprice_history_.push_back(bbo_mid);
}

void ImGuiBBOViewer::AddToHistoryImb(double imb) {
    imb_history_.push_back(imb);
}

// Convert midprice history deque to double buffer (with clamp)
const double* ImGuiBBOViewer::midprice_history_data() {
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
            midprice_buffer_[i++] = static_cast<double>(*it);
        }
    }
    return midprice_buffer_.data();
}

// Convert BBO-weighted midprice history deque to double buffer (with clamp)
const double* ImGuiBBOViewer::bbo_midprice_history_data() {
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
            bbo_midprice_buffer_[i++] = static_cast<double>(*it);
        }
    }
    return bbo_midprice_buffer_.data();
}


const double* ImGuiBBOViewer::imb_history_data() {
    const size_t MAX_HISTORY_POINTS = 5000;
    size_t history_size = imb_history_.size();
    if (history_size > MAX_HISTORY_POINTS) {
        history_size = MAX_HISTORY_POINTS;
    }

    imb_buffer_.resize(history_size);

    if (history_size > 0) {
        auto start_it = imb_history_.end() - history_size;
        size_t i = 0;
        for (auto it = start_it; it != imb_history_.end(); ++it) {
            imb_buffer_[i++] = static_cast<double>(*it);
        }
    }
    return imb_buffer_.data();
}


const double* ImGuiBBOViewer::bbo_time_history_data() {
    const size_t MAX_HISTORY_POINTS = 5000;
    size_t history_size = bbo_timestamp_history_.size();
    if (history_size > MAX_HISTORY_POINTS) {
        history_size = MAX_HISTORY_POINTS;
    }

    bbo_timestamp_buffer_.resize(history_size);

    if (history_size > 0) {
        auto start_it = bbo_timestamp_history_.end() - history_size;
        size_t i = 0;
        for (auto it = start_it; it != bbo_timestamp_history_.end(); ++it) {
            bbo_timestamp_buffer_[i++] = *it;
        }
    }
    return bbo_timestamp_buffer_.data();
}

const double* ImGuiBBOViewer::imb_time_history_data() {
    const size_t MAX_HISTORY_POINTS = 5000;
    size_t history_size = imb_timestamp_history_.size();
    if (history_size > MAX_HISTORY_POINTS) {
        history_size = MAX_HISTORY_POINTS;
    }

    imb_timestamp_buffer_.resize(history_size);

    if (history_size > 0) {
        auto start_it = imb_timestamp_history_.end() - history_size;
        size_t i = 0;
        for (auto it = start_it; it != imb_timestamp_history_.end(); ++it) {
            imb_timestamp_buffer_[i++] = *it;
        }
    }
    return imb_timestamp_buffer_.data();
}
        #include <ctime> // For time_t, localtime, strftime

int MyTimeFormatter(double value, char* buff, int size, void*) {
    time_t epoch_seconds = static_cast<time_t>(value); // Cast double to time_t
    struct tm* local_time = localtime(&epoch_seconds); // Convert to local time

    if (local_time) {
        // Example format: YYYY-MM-DD HH:MM:SS
        // You can choose any format string suitable for your needs
        // For a rolling plot, HH:MM:SS might be sufficient.
        return snprintf(buff, size, "%02d:%02d:%02d",
                        local_time->tm_hour,
                        local_time->tm_min,
                        local_time->tm_sec);
                        
        // Or to include date if zoomed out:
        // return strftime(buff, size, "%Y-%m-%d %H:%M:%S", local_time);
    }
    return snprintf(buff, size, "Invalid Time"); // Handle conversion failure
}
void ImGuiBBOViewer::RenderFrame() {
    try {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        
        const int MAX_EVENTS_PER_FRAME = 1000;
        while (disruptor.consume(bbo_cursor_, latest_snapshot_)) {
            AddToHistory(latest_snapshot_.midprice);
            AddToHistoryBBOMid(latest_snapshot_.weighted_midprice);
            bbo_timestamp_history_.push_back(latest_snapshot_.timestamp);
            latest_mid_ = latest_snapshot_.midprice;
            latest_bbo_mid_ = latest_snapshot_.weighted_midprice;
            has_new_data_ = true;
            last_update_time_ = ImGui::GetTime();

        }

        while (obdisruptor.consume(imb_cursor_, latest_imb_snapshot_)) {
            AddToHistoryImb(latest_imb_snapshot_.imb);
            std::cout << latest_imb_snapshot_.timestamp << std::endl;
            imb_timestamp_history_.push_back(latest_imb_snapshot_.timestamp);
            latest_imb_ = latest_imb_snapshot_.imb;

            has_new_data_ = true;
            last_update_time_ = ImGui::GetTime();

        }

        // Duplicate latest value every 50ms if no new update
        double current_time = ImGui::GetTime();
        //if (current_time - last_update_time_ >= 0.05 && !has_new_data_) {
          //  if (!midprice_history_.empty() && !bbo_midprice_history_.empty()) {
          //      AddToHistory(latest_mid_);
          //      AddToHistoryBBOMid(latest_bbo_mid_);
          //      AddToHistoryImb(latest_imb_);
            //    bbo_timestamp_history_.push_back(current_time);
               // imb_timestamp_history_.push_back(current_time);
            //    last_update_time_ = current_time;
         //   }
      //  }
        has_new_data_ = false;

        const size_t ROLLING_WINDOW = 1000;

        // Trim history to rolling window
        while (midprice_history_.size() > ROLLING_WINDOW) midprice_history_.pop_front();
        while (bbo_midprice_history_.size() > ROLLING_WINDOW) bbo_midprice_history_.pop_front();
        while (imb_history_.size() > ROLLING_WINDOW) imb_history_.pop_front();
        while (bbo_timestamp_history_.size() > ROLLING_WINDOW) bbo_timestamp_history_.pop_front();
        while (imb_timestamp_history_.size() > ROLLING_WINDOW) imb_timestamp_history_.pop_front();



        if (!midprice_history_.empty() && !bbo_midprice_history_.empty() && !imb_history_.empty() ) {
            const double* mid_data = midprice_history_data();
            const double* bbo_data = bbo_midprice_history_data();
            const double* imb_data = imb_history_data();
            const double* imb_ts_data = imb_time_history_data();
            const double* ts_data = bbo_time_history_data();
            size_t mid_size = midprice_buffer_.size();
            size_t bbo_size = bbo_midprice_buffer_.size();
            size_t imb_size = imb_buffer_.size();



            if (mid_size > 0 && bbo_size > 0 && imb_size > 0) {
                auto mid_minmax = std::minmax_element(midprice_buffer_.begin(), midprice_buffer_.end());
                auto bbo_minmax = std::minmax_element(bbo_midprice_buffer_.begin(), bbo_midprice_buffer_.end());

                double ymin = std::min(*mid_minmax.first, *bbo_minmax.first) * 0.9999;
                double ymax = std::max(*mid_minmax.second, *bbo_minmax.second) * 1.0001;
        

                double y_range = ymax - ymin;
                double mid_current = midprice_history_.back();
                bool need_reset = false;
                double latest_timestamp = 0.0;
                // Make sure these deques are not empty before accessing .back()
                if (!bbo_timestamp_history_.empty()) {
                    latest_timestamp = bbo_timestamp_history_.back();
                }
                if (!imb_timestamp_history_.empty()) {
                    latest_timestamp = std::max(latest_timestamp, imb_timestamp_history_.back());
                }
                const double VISIBLE_TIME_WINDOW_SEC = 60.0; 

                // Calculate x-axis limits for a rolling window
                double xmax_plot = latest_timestamp;
                double xmin_plot = latest_timestamp - VISIBLE_TIME_WINDOW_SEC;
      


                // Re-axis if latest midprice goes within 1% of y-limits
               // if (y_range > 0.0f) {
                //    double lower_dist = (mid_current - ymin) / y_range;
                //    double upper_dist = (ymax - mid_current) / y_range;
                //    if (lower_dist < 0.01f || upper_dist < 0.01f) {
                //        need_reset = true;
                 //   }
                //}

               // if (need_reset) {
                   // double mid = mid_current;
                   // ymin = mid * 0.95f;  // reset to ±5% around mid
                   // ymax = mid * 1.05f;
               // } else {
                    // Add padding for stability
                  //  double padding = y_range * 0.1f;
                  //  ymin -= padding;
                 //   ymax += padding;
                //}

                
                const size_t VISIBLE_XSCALE = 1500;  // leave room after rolling window

                // Get the available space inside ImGui window
                ImVec2 content_size = ImGui::GetContentRegionAvail();

                if (ImPlot::BeginPlot("Midprice & Weighted Midprice & Imb", content_size)) {
                   
                    ImPlot::SetupAxes(nullptr, nullptr);
                    ImPlot::SetupAxisLimits(ImAxis_X1, xmin_plot, xmax_plot, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, ImGuiCond_Always);

                    ImPlot::SetupAxisFormat(ImAxis_X1, MyTimeFormatter);
                    ImPlot::PlotLine("Midprice", ts_data,mid_data, mid_size);
                    ImPlot::PlotLine("WeightedMid", ts_data,bbo_data, bbo_size);
                    ImPlot::PlotLine("ImbMid", imb_ts_data,imb_data, imb_size);
                    

                    

                    ImPlot::EndPlot();
                }
            }
        }
    } catch (const std::length_error& e) {
        std::cerr << "Length error in RenderFrame: " << e.what() << std::endl;
        midprice_history_.clear();
        bbo_midprice_history_.clear();
        imb_history_.clear();
    } catch (const std::exception& e) {
        std::cerr << "Exception in RenderFrame: " << e.what() << std::endl;
    }
}
