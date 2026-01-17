//
// Created by Anei Markovic on 1/8/26.
//
#include "Controller/NotificationController.h"
#include "Controller/MineController.hpp"

#include "HttpServer.hpp"

#include <algorithm>

std::vector<AppMessage> NotificationController::appList = std::vector<AppMessage>();

AppMessage NotificationController::convertJsonToAppMessage(nlohmann::json json) {
    AppMessage message;

    if (json.contains("token")) message.deviceId = json["token"];
    if (json.contains("mine_name")) message.mineName = json["mine_name"];
    if (json.contains("worker_role")) message.workerType = json["worker_role"];

    if (json.contains("message_type")) {
        if (json["message_type"].is_number()) {
            message.messageType = json["message_type"];
        }
        else {
            message.messageType = std::stoi(json["message_type"].get<std::string>());
        }
    }

    if (json.contains("message")) {
        message.message = json["message"];
    }

    if (json.contains("location")) {
        message.location = json["location"];
    }

    return message;
}
std::string NotificationController::getNotificationMessageTitle(uint16_t messageType) {
    switch (messageType) {
        case 1:
            return "ALERT NOT WEARING A HELMET";
        case 2:
            return "RECEIVED A MESSAGE";
        case 3:
            return "END OF SHIFT";
        default:
            return "UNKNOWN";
    }
}
std::string NotificationController::getNotificationMessageBody(AppMessage message) {
    switch (message.messageType) {
        case 1:
            return "Worker is not wearing a helmeth, please put your helmet on";
        case 2:
            return message.message;
        case 3:
            return "Shift ends in 15 minutes";
        default:
            return "Unknown message";
    }

}
void NotificationController::receiveDeviceInfoFromApp(const request &request, response &response, Router *r) {
    std::string target = std::string(request.target());
    // std::cout << "RECEIVED REQUEST: " << target << std::endl;
    // std::cout << "SECOND TRY: " << request.body() << std::endl;

    nlohmann::json json = nlohmann::json::parse(request.body());
    // std::cout << "JSON: " << json.dump() << std::endl;

    // std::cout << "Device ID: " << json["token"] << std::endl;
    // std::cout << "Mine name: " << json["mine_name"] << std::endl;
    // std::cout << "Worker type: " << json["worker_role"] << std::endl;
    // std::cout << "Message type: " << json["message_type"] << std::endl;


    AppMessage const message = convertJsonToAppMessage(json);

    std::cout << "DEVICE FROM: [" << message.mineName << "] REGISTRED" << std::endl;

    // std::cout << "Device id: " << message.deviceId <<
    //     "\n mine name: " << message.mineName <<
    //         "\n worker type: " << message.workerType <<
    //             "\n message type: " << message.messageType << std::endl;
    /*
     * Message type = 0 => Add me to notification list
     * Message type = 1 => Not wearing a helemt
     * Message type = 2 => Sending message to other worker
     * Message type = 3 => End of shift notification (TODO: nvn ce sploh nucamo tu samo app)
     */

    // Save localy
    if (message.messageType == 0 || message.messageType == 1) {
        appList.push_back(message);
    }

    // SAVE TO BLOCKCHAIN
    MineController::saveToBlockchain("REGISTRATION_" + message.mineName + "_" + message.workerType);
}

void NotificationController::receiveMessageInfoFromApp(const request& request, response& response, Router* r) {
    nlohmann::json json = nlohmann::json::parse(request.body());
    AppMessage receivedMessage = convertJsonToAppMessage(json);

    // pripravi data
    std::string dataToMine = receivedMessage.mineName + "_" +
        receivedMessage.workerType + "_" +
        receivedMessage.location + "_" +
        receivedMessage.message;

    MineController::saveToBlockchain(dataToMine);

    NotificationController::sendMessageToUser(receivedMessage);

    // Return OK to the app immediately
    response.result(http::status::ok);
    response.body() = "Message received and saving to blockchain.";
    response.prepare_payload();
}

void NotificationController::sendMessageToUser(AppMessage receivedMessage) {
    std::vector<AppMessage> messagesToSend;
    for (AppMessage message : appList) {
        bool mineFlag = message.mineName == receivedMessage.mineName;
        bool workerFlag = message.workerType == receivedMessage.workerType;
        // bool messageFlag = message.messageType == messageType;

        if (mineFlag && (receivedMessage.messageType == 2 || workerFlag || receivedMessage.messageType == 1)){
            messagesToSend.push_back(message);
        }
    }

    try {
        boost::asio::io_context ioc;
        boost::asio::ssl::context ctx {boost::asio::ssl::context::tlsv12_client};
        boost::asio::ip::tcp::resolver resolver{ ioc };

        boost::beast::ssl_stream<boost::beast::tcp_stream> stream{ ioc, ctx };
        get_lowest_layer(stream).expires_after(std::chrono::seconds(30));
        //Server name indication
        if (!SSL_set_tlsext_host_name(stream.native_handle(), "fcm.googleapis.com")) {
            throw std::runtime_error("ERROR with server name indication");
        }
        boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> const results = resolver.resolve("fcm.googleapis.com", "443");

        boost::beast::get_lowest_layer(stream).connect(results);
        stream.handshake(boost::asio::ssl::stream_base::client);

        std::string target = "/v1/projects/" + HttpServer::projectId +"/messages:send";

        for (const auto& message: messagesToSend) {
            nlohmann::json data;
            data["message"] = {
                {"token", message.deviceId},
                {"notification", {
                    {"title", NotificationController::getNotificationMessageTitle(receivedMessage.messageType)},
                    {"body", NotificationController::getNotificationMessageBody(receivedMessage)}
                }}
            };
            std::string bodyData = data.dump();

            http::request<http::string_body> req{http::verb::post, target, 11};
            req.set(http::field::host, "fcm.googleapis.com");
            req.set(http::field::user_agent, "Rudniki/1.0");
            req.set(http::field::content_type, "application/json");
            req.set(http::field::authorization, "Bearer " + HttpServer::accessToken);

            req.keep_alive(true);

            req.body() = bodyData;
            req.prepare_payload();
            try {
                http::write(stream, req);
            } catch (std::exception &e) {
                std::cerr << "ERROR sending JWT to OAuth: " << e.what() << std::endl;
                throw std::runtime_error("ERROR sending jwt to OAuth");
            }

            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            try {
                http::read(stream, buffer, res);
            } catch (std::exception &e) {
                std::cerr << "ERROR reading response from OAuth: " << e.what() << std::endl;
                throw std::runtime_error("ERROR reading response from OAuth");
            }
            // std::cout << "OAuth response:  "<< res.body() << std::endl;
            data = nlohmann::json::parse(res.body());

            if (res.need_eof()) {
                //Connection closed;
                break;
            }
        }

        beast::error_code ec;
        stream.shutdown(ec);
    } catch (std::exception &e) {
        std::cerr << "ERROR sending JWT: " << e.what() << std::endl;
    }
}