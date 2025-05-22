//  Created by Anei Markovič 22.5.2025

#include "Model/WorkerModel.hpp"

WorkerModel::WorkerModel(std::string firstName, std::string lastName, timeStamp birthDate, int IDNumber, WorkerType type, double salary) : firstName(firstName), lastName(lastName), birthDate(birthDate), IDNumber(IDNumber), type(type), salary(salary) {}
WorkerModel::WorkerModel()
{
    this->firstName = "";
    this->lastName = "";
    this->birthDate = std::chrono::milliseconds::zero();
    this->IDNumber = 0;
    this->type = WorkerType::UNDEFINED;
    this->salary = 0.00;
}
/*
    Funkcija zgradi WorkerModel objekt iz pridobljenega BSON dokumenta
*/
void WorkerModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        this->firstName = extractStringFromBSON(docView, "firstName");
        this->lastName = extractStringFromBSON(docView, "lastName");
        this->birthDate = extractDateFromBSON(docView, "birthDate");
        this->IDNumber = extractIntFromBSON(docView, "IDNumber");
        this->type = static_cast<WorkerType>(extractIntFromBSON(docView, "type"));
        this->salary = extractDoubleFromBSON(docView, "salary");
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for WorkerModel: " << exception.what() << std::endl;
    }
}
/*
    Funkcija vzame ime polja v BSON in ga da v string format za uporabo.
    Če polja ni vrne error
*/
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
/*
    Funkcija vzame ime polja v BSON in ga da v double format za uporabo
    Če polja ni vrne error
*/
double WorkerModel::extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_double)
            {
                return element.get_double().value;
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
/*
    Funkcija vzame ime polja v BSON in ga da v int format za uporabo
    Če polja ni vrne error
*/
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
/*
    Funkcija vzame ime polja v BSON in ga da v date format za uporabo
    Če polja ni vrne error
*/
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
            else if (element.type() == bsoncxx::type::k_int64)
            {
                return std::chrono::milliseconds(element.get_int64().value);
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
/*
    Funkcija zgradi BSON dokument iz trenutnega WorkerModel objekta
*/
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
/*
    Funkcija, ki preveri oz validira podatke delavca
*/
bool WorkerModel::validateWorkers() const
{
    if (type > WorkerType::UNDEFINED || type < WorkerType::MINER)
    {
        return false;
    }

    return true;
}