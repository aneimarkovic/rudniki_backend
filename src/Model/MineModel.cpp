//  Created by Anei Markovič 22.5.2025
#include "Model/MineModel.hpp"

MineModel::MineModel(std::string name, bsoncxx::oid ownerId, mineStatus status, mineType type, std::vector<MineralModel> minerals, std::vector<InfrastructureModel> infrastructure, std::vector<WorkerModel> workers, timeStamp createdAt, timeStamp modifiedAt)
    : name(name),
      ownerId(ownerId),
      status(status),
      type(type),
      minerals(minerals),
      infrastructure(infrastructure),
      workers(workers),
      ModelTemplate(createdAt, modifiedAt)
{
    this->validateMineData();
}
/*
    Funkcija zgradi MineModel objekt iz pridobljenega BSON dokumenta
*/
void MineModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        this->id = docView["_id"].get_oid().value;
        this->name = extractStringFromBSON(docView, "name");
        this->ownerId = docView["ownerId"].get_oid().value;
        this->status = extractIntFromBSON(docView, "status");
        this->type = extractIntFromBSON(docView, "type");

        bsoncxx::array::view tempMinerals = docView["minerals"].get_array().value;
        for (const auto &item : tempMinerals)
        {
            MineralModel temp;
            temp.getFromBsonDocument(item.get_document().view());
            this->minerals.push_back(temp);
        }

        bsoncxx::array::view tempInfrastructure = docView["infrastructure"].get_array().value;
        for (const auto &item : tempInfrastructure)
        {
            infrastructureModel temp;
            temp.getFromBsonDocument(item.get_document().view());
            this->infrastructure.push_back(temp);
        }

        bsoncxx::array::view tempWorkers = docView["workers"].get_array().value;
        for (const auto &item : tempWorkers)
        {
            WorkerModel temp;
            temp.getFromBsonDocument(item.get_document().view());
            this->workers.push_back(temp);
        }
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for MineModel: " << exception.what() << std::endl;
    }
}

bsoncxx::document::value MineModel::convertToBsonDocument()
{
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("_id", id));
    builder.append(bsoncxx::builder::basic::kvp("name", this->name));
    builder.append(bsoncxx::builder::basic::kvp("ownerId", this->ownerId));
    builder.append(bsoncxx::builder::basic::kvp("status", this->status));
    builder.append(bsoncxx::builder::basic::kvp("type", this->type));
    builder.append(bsoncxx::builder::basic::kvp("created", bsoncxx::types::b_date{created}));
    builder.append(bsoncxx::builder::basic::kvp("modified", bsoncxx::types::b_date{modified}));
    bsoncxx::builder::basic::array mineralsArr{};
    for (const auto &item : this->minerals)
    {
        mineralsArr.append(item.convertToBsonDocument().view());
    }
    bsoncxx::builder::basic::array infrastructureArr{};
    for (const auto &item : this->infrastructure)
    {
        infrastructureArr.append(item.convertToBsonDocument().view());
    }
    bsoncxx::builder::basic::array workersArr{};
    for (const auto &item : this->workers)
    {
        workersArr.append(item.convertToBsonDocument().view());
    }
    return builder.extract();
}

bool MineModel::validateMineData() const
{
    if (this->type > DEEPSEA || this->type < SURFACE)
    {
        return false;
    }

    if (this->status > BUILDING || this->status < ACTIVE)
    {
        return false;
    }

    for (MineralModel item : this->minerals)
    {
        if (!item.validateMinerals())
        {
            return false;
        }
    }

    for (InfrastructureModel item : this->infrastructure)
    {
        if (!validateInfrastructure())
        {
            return false;
        }
    }

    for (WorkersModel item : this->workers)
    {
        if (!validateWorkers())
        {
            return false;
        }
    }
    return true;
}