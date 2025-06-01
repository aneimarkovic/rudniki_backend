/*
 Created by Anei Markovič 22.5.2025

Razred MineModel skrbi za shranjevanje podatkov rudnika in za manjše naloge
v zvezi z delom z podatki o rudniku

*/
#ifndef MINEMODEL_H
#define MINEMODEL_H

#include <iostream>
#include <vector>
#include <regex>

#include <bsoncxx/types.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>

#include "Model/ModelTemplate.hpp"
#include "Model/MineralModel.hpp"
#include "Model/InfrastructureModel.hpp"
#include "Model/WorkerModel.hpp"

enum class MineStatus
{
    ACTIVE,
    IDLE,
    CLOSED,
    BUILDING
};

enum class MineType
{
    SURFACE,
    UNDERGROUND,
    PLACER,
    INSITU,
    DEEPSEA
};

class MineModel : public ModelTemplate
{
private:
    bsoncxx::oid id;
    std::string name;
    std::string municipality;
    int year;
    double lon;
    double lat;
    bsoncxx::oid ownerId;
    MineStatus status;
    MineType type;
    std::vector<MineralModel> minerals;
    std::vector<InfrastructureModel> infrastructure;
    std::vector<WorkerModel> workers;

public:
    MineModel(std::string name, bsoncxx::oid ownerId, MineStatus status, MineType type, std::vector<MineralModel> minerals, std::vector<InfrastructureModel> infrastructure, std::vector<WorkerModel> workers, timeStamp createdAt, timeStamp modifiedAt, std::string municipality, int year, double lon, double lat);
    MineModel();
    void setLon(double newLon);
    void setLat(double newLat);

    void getFromBsonDocument(const bsoncxx::document::view &docView) override;
    bsoncxx::document::value convertToBsonDocument() override;

    bool validateMineData() const;
};
#endif