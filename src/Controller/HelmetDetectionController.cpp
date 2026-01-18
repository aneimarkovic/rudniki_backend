//
// Created by Anei Markovic on 1/7/26.
//
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <map>

#include "Controller/HelmetDetectionController.h"
#include "DatabaseHandler.hpp"
#include "Controller/NotificationController.h"
#include "Controller/MineController.hpp"

void HelmetDetectionController::getCVAlgorithmData(const request& request, response& response, Router* r) {
    // 1. Parse the incoming JSON
    const bsoncxx::document::value document = bsoncxx::from_json(request.body());

    // Basic validation
    if (document.view().find("password") == document.view().end()) {
        response.body() = "Missing password field";
        response.result(http::status::bad_request);
        return;
    }

    // 2. Extract Request Data
    std::string providedPassword = std::string(document.view()["password"].get_string().value);
    int totalPersons = document.view()["persons"].get_int32().value;
    int helmetsOn = document.view()["helmets"].get_int32().value;
    bool status = document.view()["status"].get_bool().value;
    bool helmetsMissing = totalPersons > helmetsOn;

    // 3. Find Mine via Password using DatabaseHandler
    auto passwordFilter = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("passwords.password", providedPassword)
    );

    auto result = DatabaseHandler::fetchSingleDocument("appPasswords", passwordFilter);

    if (!result) {
        response.body() = "Invalid Password / Mine not found";
        response.result(http::status::unauthorized);
        return;
    }

    bsoncxx::oid mineID = result->view()["mineID"].get_oid().value;
    int reporterWorkerType = 0; 

    std::map<int, std::string> workerPasswords;

    auto passwordsArray = result->view()["passwords"].get_array().value;

    for (const auto& element : passwordsArray) {
        std::string currentPass = std::string(element["password"].get_string().value);
        int currentWorkerType = element["workerType"].get_int32().value;

        workerPasswords[currentWorkerType] = currentPass;

        if (currentPass == providedPassword) {
            reporterWorkerType = currentWorkerType;
        }
    }

    bsoncxx::builder::stream::document doc{};
    doc << "mineID" << mineID
        << "status" << status
        << "totalPersons" << totalPersons
        << "helmetsOn" << helmetsOn
        << "missingHelmet" << helmetsMissing
        << "reporterWorkerType" << reporterWorkerType
        << "timestamp" << bsoncxx::types::b_date(std::chrono::system_clock::now());

    bsoncxx::document::value documentValue = doc << bsoncxx::builder::stream::finalize;

    std::optional<bsoncxx::oid> id = DatabaseHandler::insertDocumentGetInsertId("helmet_detections", documentValue);

    if (id) {
        bsoncxx::builder::stream::document respDoc{};
        respDoc << "message" << "Saved successfully"
            << "id" << id->to_string()
            << "mineID" << mineID.to_string()
            << bsoncxx::builder::stream::finalize;

        response.body() = bsoncxx::to_json(respDoc.view());
    }
    else {
        response.body() = "Error saving detection data!";
        response.result(http::status::internal_server_error);
        return;
    }

    if (helmetsMissing && id)
    {
        std::string violationData = "ALERT_HELMET_MISSING_" + mineID.to_string() + "_" + id->to_string();
        MineController::saveToBlockchain(violationData);

        for (auto passwordObject : workerPasswords) {
            AppMessage alertMessage;
            alertMessage.mineName = passwordObject.second;
            alertMessage.workerType = std::to_string(passwordObject.first);
            alertMessage.messageType = 1;
            alertMessage.message = "ALERT: Workers detected without helmets!";

            NotificationController::sendMessageToUser(alertMessage);
        }
    }

    response.result(http::status::ok);
    response.prepare_payload();
}