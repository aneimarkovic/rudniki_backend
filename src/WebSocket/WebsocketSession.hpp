// WebSocketSession.hpp
#pragma once

#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <bsoncxx/json.hpp>
#include <unordered_set>
#include <map>
#include <shared_mutex>
#include <memory>
#include <iostream>
#include <queue>
#include <mutex>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    using WebsocketStream = websocket::stream<beast::tcp_stream>;
    using WebsocketStreamPointer = std::shared_ptr<WebsocketStream>;

    

    WebSocketSession(WebsocketStreamPointer websocket, const std::string& webhookName)
        : webSocket(std::move(websocket)), webhook_name_(webhookName), writing_(false)
    {
        std::cout << "New WebSocket connection: " << webhook_name_ << std::endl;
    }

    void run() {
        if (!webSocket || !webSocket->is_open()) return;

        webSocket->async_read(
            buffer_,
            beast::bind_front_handler(
                &WebSocketSession::on_read,
                shared_from_this()
            )
        );
    }

    bool is_open() const {
        return webSocket && webSocket->is_open();
    }

    void send_message(const std::string& message) {
        if (!webSocket) {
            return;
        }

        if (!webSocket->is_open()) {
         
            return;
        }

        try {
            std::lock_guard<std::mutex> lock(write_mutex_);

            webSocket->text(true);

            beast::error_code ec;
            size_t bytes_written = webSocket->write(net::buffer(message), ec);
        }
        catch (const std::exception& e) {
            std::cout << "=== Exception in send_message: " << e.what() << std::endl;
        }
    }

    void close() {
        if (webSocket && webSocket->is_open()) {
            beast::error_code ec;
            webSocket->close(websocket::close_code::normal, ec);
        }
    }

private:
    void do_write(const std::string& message) {
        write_queue_.push(message);

        if (!writing_) {
            start_write();
        }
    }

    void start_write() {
        if (write_queue_.empty() || !is_open()) {
            writing_ = false;
            return;
        }

        writing_ = true;

        current_message_ = write_queue_.front();
        write_queue_.pop();

        webSocket->async_write(
            net::buffer(current_message_),
            [self = shared_from_this()](beast::error_code ec, std::size_t bytes_written) {
                std::cout << "Write completed, bytes: " << bytes_written << ", error: " << ec.message() << std::endl;
                self->on_write(ec);
            }
        );
    }

    void on_write(beast::error_code ec) {
        writing_ = false;

        if (ec) {
            std::cout << "Write error: " << ec.message() << std::endl;
            close();
            return;
        }

        start_write();
    }

    void on_read(beast::error_code ec, std::size_t) {
        if (ec) {
            std::cout << "Read error: " << ec.message() << std::endl;
            close();
            return;
        }

        handle_message();
        buffer_.consume(buffer_.size());
        run(); 
    }

    void handle_message() {
        std::string message = beast::buffers_to_string(buffer_.data());
        std::cout << "Received from " << webhook_name_ << ": " << message << std::endl;
    }

    WebsocketStreamPointer webSocket;
    std::string webhook_name_;
    beast::flat_buffer buffer_;

    std::queue<std::string> write_queue_;
    std::string current_message_; 
    bool writing_;
    mutable std::mutex write_mutex_;
};