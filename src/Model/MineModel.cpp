//  Created by Anei Markovič 22.5.2025
#include "Model/MineModel.hpp"

MineModel::MineModel(std::string name, bsoncxx::oid ownerId, MineStatus status, MineType type, std::vector<MineralModel> minerals, std::vector<InfrastructureModel> infrastructure, std::vector<WorkerModel> workers, timeStamp createdAt, timeStamp modifiedAt)
    : name(name),
      ownerId(ownerId),
      status(status),
      type(type),
      minerals(minerals),
      infrastructure(infrastructure),
      workers(workers),
      ModelTemplate(createdAt, modifiedAt)
{
    // validateMineData();
}

MineModel::MineModel()
    : ModelTemplate(GET_NOW_IN_MILLISECONDS(), GET_NOW_IN_MILLISECONDS())
{
    this->name = "";
    this->ownerId = bsoncxx::oid();
    this->status = MineStatus::CLOSED;
    this->type = MineType::SURFACE;
    this->minerals = {};
    this->infrastructure = {};
    this->workers = {};
}
/*
    Funkcija zgradi MineModel objekt iz pridobljenega BSON dokumenta
*/
void MineModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        auto checkID = docView["_id"];
        if (checkID && checkID.type() == bsoncxx::type::k_oid)
        {
            this->id = docView["_id"].get_oid().value;
        }
        else
        {
            this->id = bsoncxx::oid();
        }
        this->name = extractStringFromBSON(docView, "name");

        // this->ownerId = docView["ownerId"].get_oid().value;

        auto ownerIdElement = docView["ownerId"];
        if (ownerIdElement && ownerIdElement.type() == bsoncxx::type::k_oid)
        {
            this->ownerId = ownerIdElement.get_oid().value;
        }
        else if (ownerIdElement && ownerIdElement.type() == bsoncxx::type::k_string)
        {
            try
            {
                auto sv = ownerIdElement.get_string().value;
                std::string str_val(sv.data(), sv.size());
                this->ownerId = bsoncxx::oid(str_val);
            }
            catch (...)
            {
                this->ownerId = bsoncxx::oid();
            }
        }
        else
        {
            this->ownerId = bsoncxx::oid();
        }

        this->status = static_cast<MineStatus>(extractIntFromBSON(docView, "status"));
        this->type = static_cast<MineType>(extractIntFromBSON(docView, "type"));

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
            InfrastructureModel temp;
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
    builder.append(bsoncxx::builder::basic::kvp("status", static_cast<int>(this->status)));
    builder.append(bsoncxx::builder::basic::kvp("type", static_cast<int>(this->type)));
    builder.append(bsoncxx::builder::basic::kvp("created", bsoncxx::types::b_date{created}));
    builder.append(bsoncxx::builder::basic::kvp("modified", bsoncxx::types::b_date{modified}));

    auto mineralsArr = bsoncxx::builder::basic::array{};
    for (auto &item : this->minerals)
    {
        mineralsArr.append(item.convertToBsonDocument().view());
    }
    builder.append(bsoncxx::builder::basic::kvp("minerals", mineralsArr));
    auto infrastructureArr = bsoncxx::builder::basic::array{};
    for (auto &item : this->infrastructure)
    {
        infrastructureArr.append(item.convertToBsonDocument().view());
    }
    builder.append(bsoncxx::builder::basic::kvp("infrastructure", infrastructureArr));
    auto workersArr = bsoncxx::builder::basic::array{};
    for (auto &item : this->workers)
    {
        workersArr.append(item.convertToBsonDocument().view());
    }
    builder.append(bsoncxx::builder::basic::kvp("workers", workersArr));
    return builder.extract();
}

bool MineModel::validateMineData() const
{
    if (this->type > MineType::DEEPSEA || this->type < MineType::SURFACE)
    {
        return false;
    }

    if (this->status > MineStatus::BUILDING || this->status < MineStatus::ACTIVE)
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
        if (!item.validateInfrastructure())
        {
            return false;
        }
    }

    for (WorkerModel item : this->workers)
    {
        if (!item.validateWorkers())
        {
            return false;
        }
    }
    return true;
}

std::string MineModel::toString() const
{
}