//
// Created by Anei Markovič on 25. 5. 25.
//

#ifndef RUDNIKI_BACKEND_POINTMODEL_HPP
#define RUDNIKI_BACKEND_POINTMODEL_HPP
#include <iostream>
#include <bsoncxx/types.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/basic/document.hpp>

class PointModel{
private:
    double lat;
    double lon;
public:
    PointModel(double lat, double lon);
    PointModel();

    double getLat();
    double getLon();
    std::string toString();
    void getFromBsonDocument(const bsoncxx::document::view& docView);
    double extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key);
    bsoncxx::document::value convertToBsonDocument();
};
#endif //RUDNIKI_BACKEND_POINTMODEL_HPP
