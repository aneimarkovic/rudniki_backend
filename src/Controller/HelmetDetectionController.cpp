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
#include "Model/HelmetDataModel.hpp"

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

    std::map<int, std::string> workerPasswords;

    auto passwordsArray = result->view()["passwords"].get_array().value;

    for (const auto& element : passwordsArray) {
        std::string currentPass = std::string(element["password"].get_string().value);
        int currentWorkerType = element["workerType"].get_int32().value;

        workerPasswords[currentWorkerType] = currentPass;
    }

    bsoncxx::builder::stream::document doc{};
    doc << "mineID" << mineID
        << "totalPersons" << totalPersons
        << "helmetsOn" << helmetsOn
        << "missingHelmet" << helmetsMissing
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


// Send data for the users specific mine
void HelmetDetectionController::getHelmetDataForUsersMine(const request& request, response& response, Router* r)
{
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    std::string token = "";
    auto it = request.find(boost::beast::http::field::cookie);
    if (it != request.end())
    {
        auto cookie_header = std::string(it->value());
        size_t pos = cookie_header.find("jwt=");
        if (pos != std::string::npos) {
            size_t start = pos + 4;
            size_t end = cookie_header.find(";", start);
            token = cookie_header.substr(start, end - start);
        }
    }

    bsoncxx::document::element id = view["id"];
    auto stringView = id.get_string().value;
    std::string str_val(stringView.data(), stringView.size());

    bsoncxx::oid mineID = bsoncxx::oid(str_val);

    auto filters = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("mineID", mineID)
    );

    auto projection = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("mineID", bsoncxx::types::b_int32{ 0 }),
        bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int32{ 0 }),
        bsoncxx::builder::basic::kvp("reporterWorkerType", bsoncxx::types::b_int32{ 0 }),
        bsoncxx::builder::basic::kvp("status", bsoncxx::types::b_int32{ 0 })
    );


    std::vector<bsoncxx::document::value> result = DatabaseHandler::fetchMultipleDocuments("helmet_detections", filters.view(), projection.view());

    std::vector<HelmetDataModel> models;
    for (const auto& doc : result) {
        HelmetDataModel model;
        model.getFromBsonDocument(doc.view());
        models.push_back(model);
    }

    int totalScans = 0;
    int totalPersons = 0;
    int totalHelmets = 0;
    int violationCount = 0;

    std::string recordsJson = "[";

    for (size_t i = 0; i < models.size(); i++) {
        totalScans++;
        totalPersons += models[i].getTotalPersons();
        totalHelmets += models[i].getHelmetsOn();

        if (models[i].getMissingHelmet()) {
            violationCount++;
        }

        recordsJson += models[i].toString();
        if (i < models.size() - 1) {
            recordsJson += ",";
        }
    }
    recordsJson += "]";

    double compliancePercentage = 0.0;
    if (totalPersons > 0) {
        compliancePercentage = (static_cast<double>(totalHelmets) / totalPersons) * 100.0;
    }

    std::string finalJson = "{";

    finalJson += "\"stats\": {";
    finalJson += "\"total_scans\": " + std::to_string(totalScans) + ",";
    finalJson += "\"total_persons\": " + std::to_string(totalPersons) + ",";
    finalJson += "\"total_helmets\": " + std::to_string(totalHelmets) + ",";
    finalJson += "\"safety_violations\": " + std::to_string(violationCount) + ",";
    finalJson += "\"compliance_percentage\": " + std::to_string(compliancePercentage);
    finalJson += "},";

    finalJson += "\"data\": " + recordsJson;

    finalJson += "}";

    response.body() = finalJson;
    response.set(boost::beast::http::field::content_type, "application/json");
}

// Send all mine data, without mineIDs for database wide statistics
void HelmetDetectionController::getHelmetDataForAllMines(const request& request, response& response, Router* r)
{
    auto filters = bsoncxx::builder::basic::make_document();
    auto projection = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("mineID", bsoncxx::types::b_int32{ 0 }),
        bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int32{ 0 })
    );

    std::vector<bsoncxx::document::value> result = DatabaseHandler::fetchMultipleDocuments(
        "helmet_detections", filters.view(), projection.view()
    );


    std::vector<HelmetDataModel> models;
    for (const auto& doc : result) {
        HelmetDataModel model;
        model.getFromBsonDocument(doc.view()); 
        models.push_back(model);
    }

    int totalScans = 0;
    int totalPersons = 0;
    int totalHelmets = 0;
    int violationCount = 0;

    std::string recordsJson = "[";

    for (size_t i = 0; i < models.size(); i++) {
        totalScans++;
        totalPersons += models[i].getTotalPersons();
        totalHelmets += models[i].getHelmetsOn();

        if (models[i].getMissingHelmet()) {
            violationCount++;
        }

        recordsJson += models[i].toString();
        if (i < models.size() - 1) {
            recordsJson += ",";
        }
    }
    recordsJson += "]";

    double compliancePercentage = 0.0;
    if (totalPersons > 0) {
        compliancePercentage = (static_cast<double>(totalHelmets) / totalPersons) * 100.0;
    }

    std::string finalJson = "{";

    finalJson += "\"stats\": {";
    finalJson += "\"total_scans\": " + std::to_string(totalScans) + ",";
    finalJson += "\"total_persons\": " + std::to_string(totalPersons) + ",";
    finalJson += "\"total_helmets\": " + std::to_string(totalHelmets) + ",";
    finalJson += "\"safety_violations\": " + std::to_string(violationCount) + ",";
    finalJson += "\"compliance_percentage\": " + std::to_string(compliancePercentage);
    finalJson += "},"; 

    finalJson += "\"data\": " + recordsJson;

    finalJson += "}";

    response.body() = finalJson;
    response.set(boost::beast::http::field::content_type, "application/json");
}

void HelmetDetectionController::getHelmetDataByDateRange(const request& request, response& response, Router* r)
{
    if (r->UrlArguments.size() < 2) {
        response.result(boost::beast::http::status::bad_request);
        response.body() = "{\"message\": \"Missing start or end timestamp.\"}";
        return;
    }

    try {
        long long startMillis = std::stoll(r->UrlArguments[0]);
        long long endMillis = std::stoll(r->UrlArguments[1]);

        std::chrono::system_clock::time_point startTp{ std::chrono::milliseconds{startMillis} };
        std::chrono::system_clock::time_point endTp{ std::chrono::milliseconds{endMillis} };

        auto filters = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("timestamp", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$gte", bsoncxx::types::b_date{ startTp }),
                bsoncxx::builder::basic::kvp("$lte", bsoncxx::types::b_date{ endTp })
            ))
        );

        auto projection = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("mineID", bsoncxx::types::b_int32{ 0 }),
            bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int32{ 0 }),
            bsoncxx::builder::basic::kvp("reporterWorkerType", bsoncxx::types::b_int32{ 0 }),
            bsoncxx::builder::basic::kvp("status", bsoncxx::types::b_int32{ 0 })
        );

        std::vector<bsoncxx::document::value> result = DatabaseHandler::fetchMultipleDocuments(
            "helmet_detections",
            filters.view(),
            projection.view()
        );

        if (result.empty()) {
            response.body() = "{\"message\": \"No data found for this time range.\", \"data\": []}";
            response.set(boost::beast::http::field::content_type, "application/json");
            return;
        }

        std::vector<HelmetDataModel> models;
        for (const auto& doc : result) {
            HelmetDataModel model;
            model.getFromBsonDocument(doc.view());
            models.push_back(model);
        }

        int totalScans = 0;
        int totalPersons = 0;
        int totalHelmets = 0;
        int violationCount = 0;
        std::string recordsJson = "[";

        for (size_t i = 0; i < models.size(); i++) 
        {
            totalScans++;
            totalPersons += models[i].getTotalPersons();
            totalHelmets += models[i].getHelmetsOn();
            if (models[i].getMissingHelmet()) violationCount++;

            recordsJson += models[i].toString();
            if (i < models.size() - 1) recordsJson += ",";
        }
        recordsJson += "]";

        double compliancePercentage = 0.0;
        if (totalPersons > 0) {
            compliancePercentage = (static_cast<double>(totalHelmets) / totalPersons) * 100.0;
        }

        std::string finalJson = "{";
        finalJson += "\"stats\": {";
        finalJson += "\"total_scans\": " + std::to_string(totalScans) + ",";
        finalJson += "\"total_persons\": " + std::to_string(totalPersons) + ",";
        finalJson += "\"total_helmets\": " + std::to_string(totalHelmets) + ",";
        finalJson += "\"safety_violations\": " + std::to_string(violationCount) + ",";
        finalJson += "\"compliance_percentage\": " + std::to_string(compliancePercentage);
        finalJson += "},";
        finalJson += "\"data\": " + recordsJson;
        finalJson += "}";

        response.body() = finalJson;
        response.set(boost::beast::http::field::content_type, "application/json");

    }
    catch (const std::exception& e) {
        response.result(boost::beast::http::status::bad_request);
        response.body() = "{\"message\": \"Invalid timestamp format.\"}";
    }
}