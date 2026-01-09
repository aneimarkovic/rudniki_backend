//
// Created by Anei Markovic on 1/7/26.
//
#include <iostream>

#include "Controller/HelmetDetectionController.h"
#include "DatabaseHandler.hpp"
#include "Controller/NotificationController.h"

void HelmetDetectionController::getCVAlgorithmData(const request& request, response& response, Router* r) {
    const bsoncxx::document::value document = bsoncxx::from_json(request.body());

    /*
        Example data:
            { "status" : true, "persons" : 5, "helmets" : 4 }

        1. Save data to database
        2. [persons] vs [helmets] to check if anyone is without a helmet
        3. Send notification if there is a discrepency

    */

    int totalPersons = document.view()["persons"].get_int32().value;
    int helmetsOn = document.view()["helmets"].get_int32().value;
    bool status = document.view()["status"].get_bool().value;
    bool helmetsMissing = totalPersons > helmetsOn;

    bsoncxx::oid mineID; // PLACEHOLDER KER OD NIKJER NE DOBIM

    bsoncxx::builder::stream::document doc{};
    doc << "mineID" << mineID
        << "status" << status
        << "totalPersons" << totalPersons
        << "helmetsOn" << helmetsOn
        << "missingHelmet" << helmetsMissing
        << "timestamp" << bsoncxx::types::b_date(std::chrono::system_clock::now());
    bsoncxx::document::value documentValue = doc << bsoncxx::builder::stream::finalize;

    std::optional<bsoncxx::oid> id = DatabaseHandler::insertDocumentGetInsertId("helmet_detections", documentValue); // SAVE

    if (id) {
        bsoncxx::builder::stream::document respDoc{};
        respDoc << "message" << "Saved successfully"
            << "id" << id->to_string()
            << bsoncxx::builder::stream::finalize;

        response.body() = bsoncxx::to_json(respDoc.view());
    }
    else {
        response.body() = "Error saving detection data!";
    }

    if (helmetsMissing && id)
    {
        AppMessage alertMessage;
        alertMessage.mineName = "PLACEHOLDER";      
        alertMessage.workerType = "";                
        alertMessage.messageType = 1;               
        alertMessage.message = "Some workers are not wearing helmets!";

        NotificationController::sendMessageToUser(alertMessage); 
    }

    response.result(http::status::ok);
    response.prepare_payload();
}