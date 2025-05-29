//  Created by Anei Markovič 22.5.2025
#include "Model/MineralModel.hpp"

MineralModel::MineralModel(MineralName name, double min, double max, MineralGrade grade) : name(name), min(min), max(max), grade(grade) {}
MineralModel::MineralModel()
{
    this->name = MineralName::UNDEFINED;
    this->min = 0.00;
    this->max = 0.00;
    this->grade = MineralGrade::UNDEFINED;
}
/*
    Funkcija zgradi MineralModel objekt iz pridobljenega BSON dokumenta
*/
void MineralModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        this->name = static_cast<MineralName>(extractIntFromBSON(docView, "name"));
        this->min = extractDoubleFromBSON(docView, "min");
        this->max = extractDoubleFromBSON(docView, "max");
        this->grade = static_cast<MineralGrade>(extractIntFromBSON(docView, "grade"));
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for MineralModel: " << exception.what() << std::endl;
    }
}
/*
    Funkcija vzame ime polja v BSON in ga da v string format za uporabo.
    Če polja ni vrne error
*/
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
/*
    Funkcija vzame ime polja v BSON in ga da v double format za uporabo
    Če polja ni vrne error
*/
double MineralModel::extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key)
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
/*
    Funkcija zgradi BSON dokument iz trenutnega MineralModel objekta
*/
bsoncxx::document::value MineralModel::convertToBsonDocument()
{
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("name",  static_cast<int>(this->name)));
    builder.append(bsoncxx::builder::basic::kvp("min", this->min));
    builder.append(bsoncxx::builder::basic::kvp("max", this->max));
    builder.append(bsoncxx::builder::basic::kvp("grade", static_cast<int>(this->grade)));
    return builder.extract();
}
/*
    Funkcija, ki preveri oz validira podatke mineralov
*/
bool MineralModel::validateMinerals() const
{
    if (grade > MineralGrade::UNDEFINED || grade < MineralGrade::LOW)
    {
        return false;
    }

    return true;
}
