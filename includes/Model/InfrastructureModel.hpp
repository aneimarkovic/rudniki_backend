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

using timeStamp = std::chrono::milliseconds;

enum InfrastructureStatus
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
    float avgFuelConsumption;
    InfrastructureStatus status;
    timeStamp lastMaintenance;
    float operatingHours;
    int kilometer;

    InfrastructureModel(std::string brand, std::string model, int IDNumber, float avgFuelConsumption, InfrastructureStatus status, timeStamp lastMaintenance, float operatingHours, int kilometer);
    InfrastructureModel();

    void getFromBsonDocument(const bsoncxx::document::view &docView);
    std::string extractStringFromBSON(const bsoncxx::document::view &docView, const char *key);
    float extractFloatFromBSON(const bsoncxx::document::view &docView, const char *key);
    int extractIntFromBSON(const bsoncxx::document::view &docView, const char *key);
    timeStamp extractDateFromBSON(const bsoncxx::document::view &docView, const char *key);
    bsoncxx::document::value convertToBsonDocument();

    bool validateInfrastructure() const;
};
#endif