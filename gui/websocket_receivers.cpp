#include "websocket_receivers.hpp"
#include <boost/beast/core.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ===================================================================================
//
//                              Constructor & Destructor
//
// ===================================================================================

WebSocketReceiver::WebSocketReceiver(ImGuiBBOViewer& viewer) : viewer_(viewer) {}

WebSocketReceiver::~WebSocketReceiver() {
    Disconnect();
}

// ===================================================================================
//
//                              Public Control Methods
//
// ===================================================================================

/**
 * @brief Connects to the specified WebSocket server and starts the listener thread.
 */
void WebSocketReceiver::Connect(const std::string& host, const std::string& port) {
    if (is_connected_) {
        return; // Already connected
    }

    try {
        // Create a resolver to turn the host and port into a list of endpoints
        tcp::resolver resolver{ioc_};
        auto results = resolver.resolve(host, port);

        // Create the WebSocket stream and connect the underlying TCP socket
        ws_stream_ = std::make_unique<websocket::stream<tcp::socket>>(ioc_);
        net::connect(ws_stream_->next_layer(), results.begin(), results.end());

        // Perform the WebSocket handshake
        std::string host_header = host + ":" + port;
        ws_stream_->handshake(host_header, "/");

        is_connected_ = true;
        std::cout << "[WebSocket] Connected successfully to " << host << ":" << port << std::endl;

        // Start the listener thread to begin reading messages
        listener_thread_ = std::thread(&WebSocketReceiver::Listen, this);

    } catch (const std::exception& e) {
        is_connected_ = false;
        std::cerr << "[WebSocket] Connection failed: " << e.what() << std::endl;
    }
}

/**
 * @brief Disconnects from the server and joins the listener thread.
 */
void WebSocketReceiver::Disconnect() {
    if (!is_connected_) {
        return;
    }

    is_connected_ = false; // Signal the listener thread to stop
    if (ws_stream_ && ws_stream_->is_open()) {
        boost::system::error_code ec;
        ws_stream_->close(websocket::close_code::normal, ec);
        if (ec) {
            std::cerr << "[WebSocket] Error on close: " << ec.message() << std::endl;
        }
    }

    if (listener_thread_.joinable()) {
        listener_thread_.join();
    }

    std::cout << "[WebSocket] Disconnected." << std::endl;
}

bool WebSocketReceiver::IsConnected() const {
    return is_connected_;
}


// ===================================================================================
//
//                                 Internal Methods
//
// ===================================================================================

/**
 * @brief The main listener loop that runs on its own thread.
 */
void WebSocketReceiver::Listen() {
    try {
        while (is_connected_) {
            boost::beast::flat_buffer buffer;
            ws_stream_->read(buffer); // Block until a message is received
            
            // Convert the message to a string and parse it
            std::string message = boost::beast::buffers_to_string(buffer.data());
            ParseMessage(message);
        }
    } catch (const boost::system::system_error& e) {
        // This exception is often thrown when the connection is closed, which is expected.
        if (e.code() != websocket::error::closed) {
            std::cerr << "[WebSocket] Listener error: " << e.what() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[WebSocket] Listener exception: " << e.what() << std::endl;
    }
    is_connected_ = false;
}

/**
 * @brief Parses an incoming JSON message and passes it to the viewer.
 */
void WebSocketReceiver::ParseMessage(const std::string& message) {
    try {
        // Parse the string into a JSON object
        auto j = json::parse(message);
        
        // Pass the raw JSON object directly to the viewer
        viewer_.OnDataReceived(j);

    } catch (const json::exception& e) {
        std::cerr << "[WebSocket] JSON parse error: " << e.what() << std::endl;
    }
}
