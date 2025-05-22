//  Created by Anei Markovič 22.5.2025

#include "Model/WorkerModel.hpp"

WorkerModel::WorkerModel(std::string firstName, std::string lastName, timeStamp birthDate, int IDNumber, WorkerType type, float salary) : firstName(firstName), lastName(lastName), birthDate(birthDate), IDNumber(IDNumber), type(type), salary(salary) {}
WorkerModel::WorkerModel()
{
    this->firstName = "";
    this->lastName = "";
    this->birthDate = std::chrono::milliseconds::zero();
    this->IDNumber = 0;
    this->type = WorkerType::UNDEFINED;
    this->salary = 0.00;
}

void WorkerModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        this->firstName = extractStringFromBSON(docView, "firstName");
        this->lastName = extractStringFromBSON(docView, "lastName");
        this->birthDate = extractDateFromBSON(docView, "birthDate");
        this->IDNumber = extractIntFromBSON(docView, "IDNumber");
        this->type = static_cast<WorkerType>(extractIntFromBSON(docView, "type"));
        this->salary = extractFloatFromBSON(docView, "salary");
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for WorkerModel: " << exception.what() << std::endl;
    }
}

std::string WorkerModel::extractStringFromBSON(const bsoncxx::document::view &docView, const char *key)
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

float WorkerModel::extractFloatFromBSON(const bsoncxx::document::view &docView, const char *key)
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
int WorkerModel::extractIntFromBSON(const bsoncxx::document::view &docView, const char *key)
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
timeStamp WorkerModel::extractDateFromBSON(const bsoncxx::document::view &docView, const char *key)
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
bsoncxx::document::value WorkerModel::convertToBsonDocument()
{
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("firstName", this->firstName));
    builder.append(bsoncxx::builder::basic::kvp("lastName", this->lastName));
    builder.append(bsoncxx::builder::basic::kvp("IDNumber", this->IDNumber));
    builder.append(bsoncxx::builder::basic::kvp("birthDate", bsoncxx::types::b_date{birthDate}));
    builder.append(bsoncxx::builder::basic::kvp("type", static_cast<int>(this->type)));
    builder.append(bsoncxx::builder::basic::kvp("salary", this->salary));

    return builder.extract();
}

bool WorkerModel::validateWorkers() const
{
    if (type > WorkerType::UNDEFINED || type < WorkerType::MINER)
    {
        return false;
    }

    return true;
}