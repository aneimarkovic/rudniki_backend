//  Created by Anei Markovič 22.5.2025
#include "Model/MineralModel.hpp"

MineralModel::MineralModel(std::string name, float min, float max, MineralGrade grade) : name(name), min(min), max(max), grade(grade) {}
MineralModel::MineralModel()
{
    this->name = "";
    this->min = 0.00;
    this->max = 0.00;
    this->grade = UNDEFINED;
}
void MineralModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        this->name = extractStringFromBSON(docView, "name");
        this->min = extractFloatFromBSON(docView, "min");
        this->max = extractFloatFromBSON(docView, "max");
        this->grade = extractIntFromBSON(docView, "grade");
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for MineralModel: " << exception.what() << std::endl;
    }
}

std::string MineralModel::extractStringFromBSON(const bsoncxx::document::view &docView, const char *key)
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

float MineralModel::extractFloatFromBSON(const bsoncxx::document::view &docView, const char *key)
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

int MineralModel::extractIntFromBSON(const bsoncxx::document::view &docView, const char *key)
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

bsoncxx::document::value MineralModel::convertToBsonDocument()
{
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("name", this->name));
    builder.append(bsoncxx::builder::basic::kvp("min", this->min));
    builder.append(bsoncxx::builder::basic::kvp("max", this->max));
    builder.append(bsoncxx::builder::basic::kvp("grade", this->grade));
    return builder.extract();
}

bool MineralModel::validateMinerals() const{
    if(grade > UNDEFINED || grade < LOW){
        return false;
    }
    
    return true;
}
