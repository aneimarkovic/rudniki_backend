//  Created by Anei Markovič 22.5.2025
#include "Model/MineralModel.hpp"

MineralModel::MineralModel(MineralName name, double min, double max, MineralGrade grade) : name(name), min(min), max(max), grade(grade) {}
MineralModel::MineralModel()
{
    this->name = MineralName::NAME_UNDEFINED;
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

//Funkcija, ki parsa grade minerala, da vrne pravilni string
std::string  MineralModel::mineralGradeToString(int grade) {
    switch (static_cast<MineralGrade>(grade)) {
    case MineralGrade::LOW: return "LOW";
    case MineralGrade::MEDIUM: return "MEDIUM";
    case MineralGrade::HIGH: return "HIGH";
    case MineralGrade::UNDEFINED: return "UNDEFINED";
    default: return "UNKNOWN_GRADE";
    }
}
//Funkcija, ki parsa name minerala, da vrne pravilni string
std::string MineralModel::mineralNameToString(int name) {
    switch (static_cast<MineralName>(name)) {
    case MineralName::NAME_UNDEFINED: return "NAME_UNDEFINED";
    case MineralName::HALKOPIRIT: return "HALKOPIRIT";
    case MineralName::BORNIT: return "BORNIT";
    case MineralName::HALKOZIN: return "HALKOZIN";
    case MineralName::PIRIT: return "PIRIT";
    case MineralName::MARKAZIT: return "MARKAZIT";
    case MineralName::SFALERIT: return "SFALERIT";
    case MineralName::GALENIT: return "GALENIT";
    case MineralName::CINABARIT: return "CINABARIT";
    case MineralName::WULFENIT: return "WULFENIT";
    case MineralName::BARIT: return "BARIT";
    case MineralName::SREBRONOSNI_GALENIT: return "SREBRONOSNI_GALENIT";
    case MineralName::SAMORODNO_ZIVO_SREBRO: return "SAMORODNO_ZIVO_SREBRO";
    case MineralName::ANTIMONIT: return "ANTIMONIT";
    case MineralName::SIDERIT: return "SIDERIT";
    case MineralName::HEMATIT: return "HEMATIT";
    case MineralName::MAGNETIT: return "MAGNETIT";
    case MineralName::LIMONIT: return "LIMONIT";
    case MineralName::PSILOMELAN: return "PSILOMELAN";
    case MineralName::PIROLUZIT: return "PIROLUZIT";
    case MineralName::URANOVA_SMOLA: return "URANOVA_SMOLA";
    case MineralName::COFFINIT: return "COFFINIT";
    case MineralName::BOKSIT: return "BOKSIT";
    case MineralName::SAMORODNO_ZLATO: return "SAMORODNO_ZLATO";
    case MineralName::TENNANTIT: return "TENNANTIT";
    case MineralName::MALAHIT: return "MALAHIT";
    case MineralName::KUPRIT: return "KUPRIT";
    case MineralName::AZURIT: return "AZURIT";
    case MineralName::REALGAR: return "REALGAR";
    case MineralName::TORIJEVI_MINERALI: return "TORIJEVI_MINERALI";
    case MineralName::CLAY: return "CLAY";
    case MineralName::GRAVEL: return "GRAVEL";
    case MineralName::LIMESTONE: return "LIMESTONE";
    case MineralName::GRANITE: return "GRANITE";
    case MineralName::MARBLE: return "MARBLE";
    case MineralName::BASALT: return "BASALT";
    case MineralName::DIORITE: return "DIORITE";
    case MineralName::QUARTZ: return "QUARTZ";
    case MineralName::SLATE: return "SLATE";
    case MineralName::PEGMATITE: return "PEGMATITE";
    case MineralName::GYPSUM: return "GYPSUM";
    case MineralName::HALITE: return "HALITE";
    case MineralName::SULFUR: return "SULFUR";
    case MineralName::FLUORITE: return "FLUORITE";
    case MineralName::KAOLIN: return "KAOLIN";
    case MineralName::FELDSPAR: return "FELDSPAR";
    case MineralName::MICA: return "MICA";
    case MineralName::CALCITE: return "CALCITE";
    case MineralName::DOLOMITE: return "DOLOMITE";
    case MineralName::SERPENTINE: return "SERPENTINE";
    case MineralName::CHLORITE: return "CHLORITE";
    case MineralName::KYANITE: return "KYANITE";
    case MineralName::CORUNDUM: return "CORUNDUM";
    case MineralName::EMERALD: return "EMERALD";
    case MineralName::RUBY: return "RUBY";
    case MineralName::SAPPHIRE: return "SAPPHIRE";
    case MineralName::OPAL: return "OPAL";
    case MineralName::TURQUOISE: return "TURQUOISE";
    case MineralName::GRAPHITE: return "GRAPHITE";
    case MineralName::PLAGIOCLASE: return "PLAGIOCLASE";
    case MineralName::ANDALUSITE: return "ANDALUSITE";
    case MineralName::SCAPOLITE: return "SCAPOLITE";
    case MineralName::TOURMALINE: return "TOURMALINE";
    case MineralName::OLIVINE: return "OLIVINE";
    default: return "UNKNOWN_MINERAL_NAME";
    }
}
