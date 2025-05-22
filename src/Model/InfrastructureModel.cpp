// Created by Anei Markovič 22.5.2025
#include "Model/InfrastructureModel.hpp"

InfrastructureModel::InfrastructureModel(std::string brand, std::string model, int IDNumber, float avgFuelConsumption, InfrastructureStatus status, timeStamp lastMaintenance, float operatingHours, int kilometer) : brand(brand), model(model), IDNumber(IDNumber), avgFuelConsumption(avgFuelConsumption), status(status), lastMaintenance(lastMaintenance), operatingHours(operatingHours), kilometer(kilometer) {}
InfrastructureModel::InfrastructureModel()
{
    this->brand = "";
    this->model = "";
    this->IDNumber = 0;
    this->avgFuelConsumption = 0.00;
    this->status = UNDEFINED;
    this->lastMaintenance = std::chrono::milliseconds::zero();
    this->operatingHours = 0.00;
    this->kilometer = 0;
}

void InfrastructureModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        this->brand = extractStringFromBSON(docView, "brand");
        this->model = extractStringFromBSON(docView, "model");
        this->IDNumber = extractIntFromBSON(docView, "IDNumber");
        this->avgFuelConsumption = extractFloatFromBSON(docView, "avgFuelConsumption");
        this->status = extractIntFromBSON(docView, "status");
        this->lastMaintenance = extractDateFromBSON(docView, "lastMaintenance");
        this->operatingHours = extractFloatFromBSON(docView, "operatingHours");
        this->kilometer = extractIntFromBSON(docView, "kilometer");
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for InfrastructureModel: " << exception.what() << std::endl;
    }
}

std::string InfrastructureModel::extractStringFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_string)
            {
                return std::string(element.get_string().value);
            }
            else
            {
                std::cerr << "Warning: Field '" << key << "' exists but is not a string type (actual type: "
                          << bsoncxx::to_string(element.type()) << ")." << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Field '" << key << "' missing in BSON document." << std::endl;
        }
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }

    return "";
}

float InfrastructureModel::extractFloatFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_double)
            {
                double tempDouble = element.get_double().value;
                return static_cast<float>(tempDouble);
            }
            else
            {
                std::cerr << "Warning: Field '" << key << "' exists but is not a double type (actual type: "
                          << bsoncxx::to_string(element.type()) << ")." << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Field '" << key << "' missing in BSON document." << std::endl;
        }
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }
}
int InfrastructureModel::extractIntFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_int32)
            {
                return element.get_int32().value;
            }
            else
            {
                std::cerr << "Warning: Field '" << key << "' exists but is not a integer type (actual type: "
                          << bsoncxx::to_string(element.type()) << ")." << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Field '" << key << "' missing in BSON document." << std::endl;
        }
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }
}
timeStamp InfrastructureModel::extractDateFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_date)
            {
                return element.get_date().value;
            }
            else
            {
                std::cerr << "Warning: Field '" << key << "' exists but is not a date type (actual type: "
                          << bsoncxx::to_string(element.type()) << ")." << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Field '" << key << "' missing in BSON document." << std::endl;
        }
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Warning: BSON exception while accessing field '" << key << "': " << e.what() << std::endl;
    }
}

bsoncxx::document::value InfrastructureModel::convertToBsonDocument()
{
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("brand", this->brand));
    builder.append(bsoncxx::builder::basic::kvp("model", this->model));
    builder.append(bsoncxx::builder::basic::kvp("IDNumber", this->IDNumber));
    builder.append(bsoncxx::builder::basic::kvp("avgFuelConsumption", this->avgFuelConsumption));
    builder.append(bsoncxx::builder::basic::kvp("status", this->status));
    builder.append(bsoncxx::builder::basic::kvp("lastMaintenance", bsoncxx::types::b_date{lastMaintenance}));
    builder.append(bsoncxx::builder::basic::kvp("operatingHours", this->operatingHours));
    builder.append(bsoncxx::builder::basic::kvp("kilometer", this->kilometer));
    return builder.extract();
}

bool InfrastructureModel::validateInfrastructure() const
{
    if (status > UNDEFINED || status < ACTIVE)
    {
        return false;
    }
    return true;
}