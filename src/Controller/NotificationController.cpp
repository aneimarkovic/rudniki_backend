//
// Created by Anei Markovic on 1/8/26.
//

#include "Controller/NotificationController.h"
#include "Controller/MineController.hpp"
#include "DatabaseHandler.hpp"
#include "HttpServer.hpp"

#include <algorithm>

std::vector<AppMessage> NotificationController::appList = std::vector<AppMessage>();

AppMessage NotificationController::convertJsonToAppMessage(nlohmann::json json) {
    AppMessage message;
    message.deviceId = json["token"];
    message.mineName = json["mine_name"];
    message.workerType = json["worker_role"];
    message.messageType = std::stoi(json["message_type"].get<std::string>());
    if (json.contains("location")) {
        message.location = json["location"];
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
            return "ALERT NO HELMET";
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
            return "Workers without helmet";
        case 2:
            return message.message;
        case 3:
            return "Shift ends in 15 minutes";
        default:
            return "Unknown message";
    }

}
void NotificationController::receiveDeviceInfoFromApp(const request& request, response& response, Router* r) {
    std::string target = std::string(request.target());

    nlohmann::json json = nlohmann::json::parse(request.body());
    AppMessage message = convertJsonToAppMessage(json);

    std::cout << "DEVICE FROM: [" << message.mineName << "] REGISTRED" << std::endl;

    /*
     * Message type = 0 => Add me to notification list
     * Message type = 1 => Not wearing a helemt
     * Message type = 2 => Sending message to other worker
     */

    std::string providedPassword = message.mineName;

    auto passwordFilter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("passwords.password", providedPassword)
    );

    auto result = DatabaseHandler::fetchSingleDocument("appPasswords", passwordFilter);

    if (!result) {
        bsoncxx::builder::stream::document responseBuilder;
        responseBuilder << "success" << false;
        response.set(http::field::content_type, "application/json");
        response.body() = bsoncxx::to_json(responseBuilder.view());
        std::cout << "VRACAM: " << response.body() << std::endl;
        response.result(boost::beast::http::status::unauthorized);
        response.prepare_payload();
        return;
    }

    bsoncxx::document::view passwordView = result->view();
    bsoncxx::oid linkedMineOid;
    bool mineIdFound = false;

    if (passwordView["mineID"] && passwordView["mineID"].type() == bsoncxx::type::k_oid) {
        linkedMineOid = passwordView["mineID"].get_oid().value;
        mineIdFound = true;
    }

    auto passwordsArray = passwordView["passwords"].get_array().value;

    for (const auto& element : passwordsArray) {
        std::string currentPass = std::string(element["password"].get_string().value);
        int currentWorkerType = element["workerType"].get_int32().value;

        if (message.mineName == currentPass)
        {
            message.workerType = std::to_string(currentWorkerType);
        }
    }

    if (message.messageType == 0) {
        bool found = false;

        for (auto& registeredApp : appList) {
            if (registeredApp.deviceId == message.deviceId) {
                registeredApp = message;
                found = true;
                std::cout << " -> Device updated (already existed)." << std::endl;
                break;
            }
        }

        if (!found) {
            appList.push_back(message);
            std::cout << " -> New device added." << std::endl;
        }
    }

    // SAVE TO BLOCKCHAIN
    MineController::saveToBlockchain("REGISTRATION_" + message.mineName + "_" + message.workerType);

    bsoncxx::builder::stream::document responseBuilder;
    responseBuilder << "success" << true;

    if (mineIdFound) {
        auto borderFilter = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("mineId", linkedMineOid)
        );

        auto borderResult = DatabaseHandler::fetchSingleDocument("bordersTest", borderFilter);

        if (borderResult) {
            bsoncxx::document::view borderView = borderResult->view();
            if (borderView["geometry"]) {
                responseBuilder << "geometry" << borderView["geometry"].get_value();
            }
            else {
                responseBuilder << "message" << "Geometry not found in bordersTest";
            }
        }
        else {
            responseBuilder << "message" << "No borders found for this mine";
        }
    }
    else {
        responseBuilder << "message" << "MineID not linked in passwords document";
    }

    // 5. Send Response
    response.result(boost::beast::http::status::ok);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.body() = bsoncxx::to_json(responseBuilder.view());
}

void NotificationController::receiveMessageInfoFromApp(const request& request, response& response, Router* r) {
    nlohmann::json json = nlohmann::json::parse(request.body());
    AppMessage receivedMessage = convertJsonToAppMessage(json);

    std::string providedPassword = receivedMessage.mineName;

    auto passwordFilter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("passwords.password", providedPassword)
    );

    auto result = DatabaseHandler::fetchSingleDocument("appPasswords", passwordFilter);

    if (!result) {
        response.body() = "Invalid Password / Mine not found";
        response.result(http::status::unauthorized);
        return;
    }

    std::string targetPassword = "";
    int targetWorkerTypeInt = std::stoi(receivedMessage.workerType);

    auto passwordsArray = result->view()["passwords"].get_array().value;

    std::map<int, std::string> workerPasswords;

    for (const auto& element : passwordsArray) {
        std::string currentPass = std::string(element["password"].get_string().value);
        int currentWorkerType = element["workerType"].get_int32().value;

        workerPasswords[currentWorkerType] = currentPass;

        if (currentWorkerType == targetWorkerTypeInt) {
            targetPassword = currentPass;
        }
    }

    if (targetPassword.empty()) {
        response.body() = "Target WorkerType does not exist in this Mine.";
        response.result(http::status::bad_request);
        return;
    }

    std::string dataToMine = providedPassword + "_" +
        receivedMessage.workerType + "_" +
        receivedMessage.location + "_" +
        receivedMessage.message;

    MineController::saveToBlockchain(dataToMine);

    receivedMessage.mineName = targetPassword;

    if (targetWorkerTypeInt == -1)
    {
        for (auto passwordObject : workerPasswords) {
            receivedMessage.mineName = passwordObject.second;
            receivedMessage.workerType = std::to_string(passwordObject.first);
            NotificationController::sendMessageToUser(receivedMessage);
        }
    }
    else
    {
        NotificationController::sendMessageToUser(receivedMessage);
    }


    response.result(http::status::ok);
    response.body() = "Message received, saved, and sent to target.";
    response.prepare_payload();
}

void NotificationController::sendMessageToUser(AppMessage receivedMessage) {
    std::vector<AppMessage> messagesToSend;
    for (const AppMessage& registeredDevice : appList) {

        if (registeredDevice.mineName != receivedMessage.mineName) {
            continue;
        }

        if (registeredDevice.workerType == receivedMessage.workerType || receivedMessage.messageType == 1) {
            messagesToSend.push_back(registeredDevice);
        }
    }

    if (messagesToSend.empty()) {
        return;
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