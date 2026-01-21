//
// Created by Anei Markovic on 1/7/26.
//

#ifndef RUDNIKI_BACKEND_HELMETDETECTIONCONTROLLER_H
#define RUDNIKI_BACKEND_HELMETDETECTIONCONTROLLER_H

#include "RouterUtil/Router.hpp"

class HelmetDetectionController {
public:
    static void getCVAlgorithmData(const request& request, response& response, Router* r);

    static void getHelmetDataForUsersMine(const request& request, response& response, Router* r);
    static void getHelmetDataForAllMines(const request& request, response& response, Router* r);
    static void getHelmetDataByDateRange(const request& request, response& response, Router* r);
};

#endif //RUDNIKI_BACKEND_HELMETDETECTIONCONTROLLER_H