#include "WebSocket/WebsocketController.hpp"
#include <bsoncxx/json.hpp>
#include <boost/beast/websocket.hpp>
#include "WebSocket/WebsocketSession.hpp" 

#include <future>     
#include <thread>       
#include <chrono>       
#include <iostream>     
#include <exception>  

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

std::map<std::string, std::unordered_set<std::shared_ptr<WebSocketSession>>> WebSocketController::clientMap{};
std::shared_mutex WebSocketController::websocketMutex{};

/*
    Funkcija dinamično ustvari routeFunction, ki se uporabi za vsak websocket posebaj. 
    Logika vzame HTTP request ter ga poviša v Websocket ter ga zažene
*/
routeFunction WebSocketController::createInternalWebSocketFunction(const std::string& webHookName) {
    return [webHookName](const request& req, response& res, Router* r) mutable {
        if (!websocket::is_upgrade(req)) {
            res.result(http::status::bad_request);
            res.set(http::field::content_type, "text/plain");
            res.body() = "WebSocket upgrade required";
            res.prepare_payload();
            return;
        }

        beast::tcp_stream* stream = r->get_stream();
        if (!stream) {
            res.result(http::status::internal_server_error);
            res.set(http::field::content_type, "text/plain");
            res.body() = "Stream not available";
            res.prepare_payload();
            return;
        }

        try {
            auto ws = std::make_shared<WebSocketSession::WebsocketStream>(std::move(*stream));

            // Set timeout options
            ws->set_option(websocket::stream_base::timeout::suggested(
                beast::role_type::server
            ));

            ws->set_option(websocket::stream_base::decorator(
                [](websocket::response_type& res) {
                    res.set(http::field::server, "RudnikServer/1.0");
                }
            ));

            // Perform the handshake
            ws->accept(req);

            // Create and track session
            auto session = std::make_shared<WebSocketSession>(ws, webHookName);
            {
                std::unique_lock lock(websocketMutex);
                clientMap[webHookName].insert(session);
            }
            session->run();

        }
        catch (const std::exception& e) {
            std::cerr << "WebSocket error: " << e.what() << std::endl;
            res.result(http::status::internal_server_error);
            res.body() = std::string("WebSocket failed: ") + e.what();
            res.prepare_payload();
        }
        };
}

/*
    Funkcija ustvari route preko Routerja, ki se uporablja za websocket
*/
void WebSocketController::createWebSocket(const std::string& websocketURL, const std::string& webhookName) {
    routeFunction wsFunction = createInternalWebSocketFunction(webhookName);
    Router::createGetRoute(websocketURL, wsFunction);
}

/*
    Funkcija prejme ime websocket-a ter paylod, ki se ga mora poslati
*/
void WebSocketController::sendBroadcast(const std::string& webhookName, const bsoncxx::document::value& payload) {

    std::string jsonMessage = bsoncxx::to_json(payload);
    std::vector<std::shared_ptr<WebSocketSession>> sessionsToMessage;
    bool cleanupNeeded = false;
    {
        std::shared_lock lock(websocketMutex);

        auto it = clientMap.find(webhookName);
        if (it == clientMap.end()) {
            return;
        }

        sessionsToMessage.reserve(it->second.size());
        for (const auto& sessionPointer : it->second) {
            if (sessionPointer && sessionPointer->is_open()) {
                sessionsToMessage.push_back(sessionPointer);
            }
            else {
                cleanupNeeded = true;
            }
        }
    }

    for (size_t i = 0; i < sessionsToMessage.size(); ++i) {

        sessionsToMessage[i]->send_message(jsonMessage);
    
    }

    if (cleanupNeeded) {
        std::unique_lock lock(websocketMutex);

        auto it = clientMap.find(webhookName);
        if (it == clientMap.end()) {
            return;
        }

        SessionSet& currentSession = it->second;
        SessionSet still_active_sessions;
        for (const auto& session_ptr : currentSession) {
            if (session_ptr && session_ptr->is_open()) {
                still_active_sessions.insert(session_ptr);
            }
        }
        currentSession = std::move(still_active_sessions);
      
    }

}


