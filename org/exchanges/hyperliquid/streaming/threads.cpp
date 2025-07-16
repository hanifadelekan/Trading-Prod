#include "subscription_patterns.hpp"
#include "core/networking/ws_reader.hpp"
#include "core/networking/resolve_ws.hpp"
#include "gui/gui_app.hpp"
#include "core/networking/websocket_aliases.hpp"
#include "gui/disruptor.h"
#include "datacentre/control.hpp"

void StartMarketDataStreams(GuiApp* app,const std::string& exchange) {


    std::cout << "Starting market data streams...\n";

    app->bbo_thread_ = std::thread([app, exchange]() {
        std::string host;
        std::string port;
        std::string target;
        std::function<void(std::shared_ptr<WebSocketType>, const std::string&, beast::error_code&)> subscribe_fn;

        // Choose exchange-specific params
        if (exchange == "hyperliquid") {
            host = "api.hyperliquid.xyz";
            port = "443";
            target = "/ws";
            subscribe_fn = [](std::shared_ptr<WebSocketType> ws, const std::string& msg, beast::error_code& ec) {
        hyperliquid_subscribe(msg, ws, ec);  // Your actual function signature
        
};

        } 
        else {
            std::cerr << "Unsupported exchange: " << exchange << std::endl;
            return;
        }

        std::cout << exchange << " hyperliquid thread started. Setting up websocket stream...\n";

        auto [ws, buffer] = start_websocket_stream(host, port, target, app->shared_io_context_, *app->shared_ssl_context_);

        if (!ws || !buffer) {
            std::cerr << "WebSocket setup failed.\n";
            return;
        }

        beast::error_code ec;
        subscribe_fn(ws, "SOL", ec);  // Use appropriate subscription function
        HJBData bids = load_hjb_solution("/Users/hanifadelekan/dev/Trading-Prod/org/datacentre/hjb_solution.h5","bid_half");
        HJBData asks = load_hjb_solution("/Users/hanifadelekan/dev/Trading-Prod/org/datacentre/hjb_solution.h5","ask_half");
        if (ec) {
            std::cerr << "Subscription failed: " << ec.message() << "\n";
            return;
        }

        start_async_read(
            ws, buffer, app->shared_io_context_,"hyperliquid",
             app->order_book_,
            app->disruptor_, app->obdisruptor_,bids,asks
        );

        app->shared_io_context_->run();
        std::cout << "BBO thread io_context finished.\n";
    });

    app->bbo_viewer_ = std::make_unique<ImGuiBBOViewer>(app->window_, app->disruptor_, app->obdisruptor_);


    
    app->hl_market_data_started_ = true;
    app->show_bbo_snapshot_window_ = true;

}


void StartTrading(std::string exchange) {

}