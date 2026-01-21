#ifndef RUDNIKI_BACKEND_HELMETDATAMODEL_HPP
#define RUDNIKI_BACKEND_HELMETDATAMODEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <bsoncxx/types.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

class HelmetDataModel {
private:
    int totalPersons;
    int helmetsOn;
    bool missingHelmet;
    long long timestamp; 

public:
    HelmetDataModel(int totalPersons, int helmetsOn, bool missingHelmet, long long timestamp);
    HelmetDataModel();

    int getTotalPersons() const;
    int getHelmetsOn() const;
    bool getMissingHelmet() const;
    long long getTimestamp() const;

    std::string toString();

    void getFromBsonDocument(const bsoncxx::document::view& docView);
    bsoncxx::document::value convertToBsonDocument();

    int extractIntFromBSON(const bsoncxx::document::view& docView, const char* key);
    bool extractBoolFromBSON(const bsoncxx::document::view& docView, const char* key);
    long long extractDateFromBSON(const bsoncxx::document::view& docView, const char* key);
};

#endif //RUDNIKI_BACKEND_HELMETDATAMODEL_HPP