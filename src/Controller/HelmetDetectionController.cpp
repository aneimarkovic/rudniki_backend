//
// Created by Anei Markovic on 1/7/26.
//
#include <iostream>

#include "Controller/HelmetDetectionController.h"

void HelmetDetectionController::getCVAlgorithmData(const request& request, response& response, Router* r) {
    const bsoncxx::document::value document = bsoncxx::from_json(request.body());

    std::cout << "Helmets: "
          << bsoncxx::to_json(document.view())
          << std::endl;

    response.result(http::status::ok);
    response.prepare_payload();
}