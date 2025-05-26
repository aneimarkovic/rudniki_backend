//
// Created by Anei Markovič on 26. 5. 25.
//

#ifndef RUDNIKI_BACKEND_MODELBORDERS_HPP
#define RUDNIKI_BACKEND_MODELBORDERS_HPP

#include <Model/PointModel.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/basic/array.hpp>

#include <optional>

class BordersModel{
public:
    bsoncxx::oid mineId;
    std::vector<PointModel> points;
public:
    BordersModel(bsoncxx::oid mineId, std::vector<PointModel> pints);
    BordersModel();

    void setMineId(bsoncxx::oid newId);

    void getFromBsonDocument(const bsoncxx::document::view& docView);
    double extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key);
    bsoncxx::document::value convertToBsonDocument();
};

#endif //RUDNIKI_BACKEND_MODELBORDERS_HPP
