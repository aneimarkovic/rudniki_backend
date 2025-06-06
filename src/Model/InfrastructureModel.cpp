// Created by Anei Markovič 22.5.2025
#include "Model/InfrastructureModel.hpp"

InfrastructureModel::InfrastructureModel(std::string brand, std::string model, int IDNumber, double avgFuelConsumption, InfrastructureStatus status, timeStamp lastMaintenance, double operatingHours, int kilometer) : brand(brand), model(model), IDNumber(IDNumber), avgFuelConsumption(avgFuelConsumption), status(status), lastMaintenance(lastMaintenance), operatingHours(operatingHours), kilometer(kilometer) {}
InfrastructureModel::InfrastructureModel()
{
    this->brand = "";
    this->model = "";
    this->IDNumber = 0;
    this->avgFuelConsumption = 0.00;
    this->status = InfrastructureStatus::UNDEFINED;
    this->lastMaintenance = std::chrono::milliseconds::zero();
    this->operatingHours = 0.00;
    this->kilometer = 0;
}

/*
    Funkcija zgradi InfrastructureModel objekt iz pridobljenega BSON dokumenta
*/
void InfrastructureModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        this->brand = extractStringFromBSON(docView, "brand");
        this->model = extractStringFromBSON(docView, "model");
        this->IDNumber = extractIntFromBSON(docView, "IDNumber");
        this->avgFuelConsumption = extractDoubleFromBSON(docView, "avgFuelConsumption");
        this->status = static_cast<InfrastructureStatus>(extractIntFromBSON(docView, "status"));
        this->lastMaintenance = extractDateFromBSON(docView, "lastMaintenance");
        this->operatingHours = extractDoubleFromBSON(docView, "operatingHours");
        this->kilometer = extractIntFromBSON(docView, "kilometer");
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for InfrastructureModel: " << exception.what() << std::endl;
    }
}

/*
    Funkcija vzame ime polja v BSON in ga da v string format za uporabo.
    Če polja ni vrne error
*/
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
/*
    Funkcija vzame ime polja v BSON in ga da v double format za uporabo
    Če polja ni vrne error
*/
double InfrastructureModel::extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key)
{
    try
    {
        bsoncxx::document::element element = docView[key];

        if (element)
        {
            if (element.type() == bsoncxx::type::k_double)
            {
                return (double)element.get_double().value;
            } else if (element.type() == bsoncxx::type::k_int32){
                return (double)element.get_int32().value;
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
/*
    Funkcija vzame ime polja v BSON in ga da v date format za uporabo
    Če polja ni vrne error
*/
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
    Funkcija zgradi BSON dokument iz trenutnega InfrastructureModel objekta
*/
bsoncxx::document::value InfrastructureModel::convertToBsonDocument()
{
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("brand", this->brand));
    builder.append(bsoncxx::builder::basic::kvp("model", this->model));
    builder.append(bsoncxx::builder::basic::kvp("IDNumber", this->IDNumber));
    builder.append(bsoncxx::builder::basic::kvp("avgFuelConsumption", this->avgFuelConsumption));
    builder.append(bsoncxx::builder::basic::kvp("status", static_cast<int>(this->status)));
    builder.append(bsoncxx::builder::basic::kvp("lastMaintenance", bsoncxx::types::b_date{lastMaintenance}));
    builder.append(bsoncxx::builder::basic::kvp("operatingHours", this->operatingHours));
    builder.append(bsoncxx::builder::basic::kvp("kilometer", this->kilometer));
    return builder.extract();
}
/*
    Funkcija, ki preveri oz validira podatke infrastrukture
*/
bool InfrastructureModel::validateInfrastructure() const
{
    if (status > InfrastructureStatus::UNDEFINED || status < InfrastructureStatus::ACTIVE)
    {
        return false;
    }
    return true;
}