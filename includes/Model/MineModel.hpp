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

#include "Model/ModelTemplate.hpp"
#include "Model/MineralModel.hpp"
#include "Model/InfrastructureModel.hpp"
#include "Model/WorkerModel.hpp"

enum mineStatus
{
    ACTIVE,
    IDLE,
    CLOSED,
    BUILDING
};

enum mineType{
    SURFACE,
    UNDERGROUND,
    PLACER,
    INSITU,
    DEEPSEA
};

class MineModel : public ModelTemplate
{
private:
    std::string name;
    bsoncxx::oid ownerId;
    mineStatus status;
    mineType type;
    std::vector<MineralModel> minerals;
    std::vector<InfrastructureModel> infrastructure;
    std::vector<WorkerModel> workers;
public:
    MineModel(std::string name, bsoncxx::oid ownerId, mineStatus status, mineType type, std::vector<MineralModel> minerals, std::vector<InfrastructureModel> infrastructure, std::vector<WorkerModel> workers, timeStamp createdAt, timeStamp modifiedAt);
    void getFromBsonDocument(const bsoncxx::document::view& docView) override;
    bsoncxx::document::value convertToBsonDocument() override;

    bool validateMineData() const;
}
#endif