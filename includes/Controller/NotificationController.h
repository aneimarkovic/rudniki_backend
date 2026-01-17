//
// Created by Anei Markovic on 1/8/26.
//

#ifndef RUDNIKI_BACKEND_NOTIFICATIONCONTROLLER_H
#define RUDNIKI_BACKEND_NOTIFICATIONCONTROLLER_H

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket.hpp>

#include <boost/beast/websocket.hpp>

#include "RouterUtil/Router.hpp"

struct AppMessage {
    std::string deviceId;
    std::string mineName;
    std::string workerType;
    uint16_t messageType;
    std::string message = "";
    std::string location = "";
};

class NotificationController {
private:
    static std::vector<AppMessage> appList;
public:
    static void receiveDeviceInfoFromApp(const request& request, response& response, Router* r);
    static void receiveMessageInfoFromApp(const request& request, response& response, Router* r);
    static void sendMessageToUser(AppMessage receivedMessage);
    static AppMessage convertJsonToAppMessage(nlohmann::json json);
    static std::string getNotificationMessageTitle(uint16_t messageType);
    static std::string getNotificationMessageBody(AppMessage message);
};
#endif //RUDNIKI_BACKEND_NOTIFICATIONCONTROLLER_H