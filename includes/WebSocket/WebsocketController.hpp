#pragma once

#include <string>                           
#include <map>                            
#include <unordered_set>                
#include <memory>                          
#include <shared_mutex>                   
#include <functional>        
#include "RouterUtil/Router.hpp"

#include <bsoncxx/document/value.hpp>      


#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

class WebSocketSession;
class Router;         

/*
    Žan Misja

    Class upravlja s vsem kar je povezano z websockets. Ustvari internal route s pomočjo Router-ja kjer se HTTP
    povezave povečajo v Websocket povezave.
*/

class WebSocketController {
public:
    using HttpRequest = boost::beast::http::request<boost::beast::http::string_body>;
    using HttpResponse = boost::beast::http::response<boost::beast::http::string_body>;

    using SessionSet = std::unordered_set<std::shared_ptr<WebSocketSession>>;

    using routeFunction = std::function<void(const HttpRequest& req, HttpResponse& res, Router* r)>;

    static routeFunction createInternalWebSocketFunction(const std::string& webHookName);

    static void createWebSocket(const std::string& websocketURL, const std::string& webhookName);

    static void sendBroadcast(const std::string& webhookName, const bsoncxx::document::value& payload);

    static void safeBroadcast(const std::string& webhookName, const bsoncxx::document::value& payload);

    WebSocketController() = delete;
    ~WebSocketController() = delete;
    WebSocketController(const WebSocketController&) = delete;
    WebSocketController& operator=(const WebSocketController&) = delete;
    WebSocketController(WebSocketController&&) = delete;
    WebSocketController& operator=(WebSocketController&&) = delete;

private:
    static std::map<std::string, SessionSet> clientMap;

    static std::shared_mutex websocketMutex;
};