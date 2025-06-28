#include "fetch_hyperliquid_bbo.h"
#include "fetch_hyperliquid_orderbook.h"
#include "imgui_bbo_viewer.h"
#include "order_manager.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <thread>
#include <iostream>
#include <implot.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <memory>
#include "fetch_hyperliquid_bbo.h"
#include "disruptor.h"
#include "hyperliquid_parser.h"
#include "order_manager.hpp"
// ✅ Global disruptor now uses BBOSnapshot, not vector<BBOLevel>
Disruptor<BBOSnapshot> disruptor(1024);

HyperliquidParser parser;

namespace net = boost::asio;
namespace ssl = net::ssl;

std::thread bbo_thread;
std::thread ob_thread;

std::shared_ptr<net::io_context> shared_io_context;
std::unique_ptr<ssl::context> shared_ssl_context;
using work_guard_type = net::executor_work_guard<net::io_context::executor_type>;
std::unique_ptr<work_guard_type> work_guard;

int main() {
    shared_io_context = std::make_shared<net::io_context>();
    shared_ssl_context = std::make_unique<ssl::context>(ssl::context::tlsv12_client);
    shared_ssl_context->set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 | ssl::context::no_sslv3 | ssl::context::no_tlsv1 | ssl::context::no_tlsv1_1 | ssl::context::single_dh_use);
    shared_ssl_context->set_default_verify_paths();
    work_guard = std::make_unique<work_guard_type>(shared_io_context->get_executor());

    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1200, 800, "Hyperliquid Dashboard", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION(); ImGui::CreateContext(); ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark(); ImGui_ImplGlfw_InitForOpenGL(window, true); ImGui_ImplOpenGL3_Init("#version 150");

    // ❌ Do NOT start BBO thread
     bbo_thread = std::thread([]() {
         std::cout << "BBO thread started. Setting up stream...\n";
         run_bbo_async_stream(shared_io_context, *shared_ssl_context, "BTC", "bbo");
        shared_io_context->run();
        std::cout << "BBO thread io_context finished.\n";
     });

    // ❌ Do NOT start Order Book thread
     ob_thread = std::thread([]() {
         std::cout << "Order Book thread started. Setting up stream...\n";
         run_orderbook_async_stream(shared_io_context, *shared_ssl_context, "BTC");
         shared_io_context->run();
        std::cout << "Order Book thread io_context finished.\n";
     });

    // ✅ Start OrderManager in main thread context
    //OrderManager order_manager;
    //order_manager.start();

    ImGuiBBOViewer viewer(window);
    OrderManager order_manager;
    order_manager.init();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGuiDockNodeFlags_None);

        // 🟢 Draw BBO Snapshot window, resizable & movable by user
        ImGui::Begin("BBO Snapshot");
        viewer.RenderFrame();
        ImGui::End();

        // 🟢 Draw Control panel
        // 🟢 Draw Control panel (force separate platform window to avoid sticking)
        ImGui::SetNextWindowViewport(0);                          // reassign to root platform window every frame
        ImGui::SetNextWindowDockID(0, ImGuiCond_Always);          // prevent docking
        ImGui::Begin("Control", nullptr, ImGuiWindowFlags_NoDocking);
        ImGui::End();


        order_manager.process();
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }

        glfwSwapBuffers(window);
}



    std::cout << "Shutting down...\n";
    //order_manager.stop();          // ✅ cleanly stop OrderManager
    work_guard.reset(); shared_io_context->stop();
    if (bbo_thread.joinable()) bbo_thread.join();
    if (ob_thread.joinable()) ob_thread.join();
    ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImPlot::DestroyContext(); ImGui::DestroyContext();
    glfwDestroyWindow(window); glfwTerminate(); return 0;
}
