/*
 Created by Anei Markovič 22.5.2025

Razred  skrbi za delo z delavci in njihovimi atributi

*/
#ifndef WORKERMODEL_H
#define WORKERMODEL_H

#include <iostream>
#include <chrono>

#include <bsoncxx/types.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/basic/document.hpp>

using timeStamp = std::chrono::milliseconds;
enum class WorkerType
{
    MINER,
    MACHINEOPERATOR,
    BLASTINGTECHNICIAN,
    MAINTENANCEWORKER,
    SURVEYOR, // GEODET
    GEOLOGIST,
    MININGENGINEER,
    SAFETYTECHNICIAN,
    VENTILATIONENGINEER,
    HYDROLOGIST,
    ELECTRICTIAN,
    SUPERVISOR,
    LABORATORYTECHNICIAN,
    DIRECTOR,
    PRODUCTIONMANAGER,
    HUMANRESOURCES,
    ACCOUNTANT,
    UNDEFINED
};
class WorkerModel
{
private:
    std::string firstName;
    std::string lastName;
    timeStamp birthDate;
    int IDNumber;
    WorkerType type;
    double salary;

public:
    WorkerModel(std::string firstName, std::string lastName, timeStamp birthDate, int IDNumber, WorkerType type, double salary);
    WorkerModel();

    void getFromBsonDocument(const bsoncxx::document::view &docView);
    std::string extractStringFromBSON(const bsoncxx::document::view &docView, const char *key);
    double extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key);
    int extractIntFromBSON(const bsoncxx::document::view &docView, const char *key);
    timeStamp extractDateFromBSON(const bsoncxx::document::view &docView, const char *key);
    bsoncxx::document::value convertToBsonDocument();

    bool validateWorkers() const;
};
#endif