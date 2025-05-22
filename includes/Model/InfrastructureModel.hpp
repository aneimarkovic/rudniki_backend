/*
 Created by Anei Markovič 22.5.2025

Razred  skrbi za delo z infrastrukturo rudnika/stroji in njihovimi atributi

*/
#ifndef INFRASTRUCTUREMODEL_H
#define INFRASTRUCTUREMODEL_H

#include <iostream>
#include "chrono"

#include <bsoncxx/types.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/document/view.hpp>

using timeStamp = std::chrono::milliseconds;

enum class InfrastructureStatus
{
    ACTIVE,
    IDLE,
    BROKEN,
    INREPAIR,
    OUTOFSERVICE,
    CLEANING,
    UNDEFINED
};

class InfrastructureModel
{
private:
    std::string brand;
    std::string model;
    int IDNumber;
    double avgFuelConsumption;
    InfrastructureStatus status;
    timeStamp lastMaintenance;
    double operatingHours;
    int kilometer;

public:
    InfrastructureModel(std::string brand, std::string model, int IDNumber, double avgFuelConsumption, InfrastructureStatus status, timeStamp lastMaintenance, double operatingHours, int kilometer);
    InfrastructureModel();

    void getFromBsonDocument(const bsoncxx::document::view &docView);
    std::string extractStringFromBSON(const bsoncxx::document::view &docView, const char *key);
    double extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key);
    int extractIntFromBSON(const bsoncxx::document::view &docView, const char *key);
    timeStamp extractDateFromBSON(const bsoncxx::document::view &docView, const char *key);
    bsoncxx::document::value convertToBsonDocument();

    bool validateInfrastructure() const;
};
#endif