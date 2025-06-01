/*
 Created by Anei Markovič 22.5.2025

Razred skrbi za delo z Minerali in njihovimi atributi

*/
#ifndef MINERALMODEL_H
#define MINERALMODEL_H

#include <iostream>
#include <string>

#include <bsoncxx/types.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/builder/basic/document.hpp>

enum class MineralGrade
{
    LOW,
    MEDIUM,
    HIGH,
    UNDEFINED
};

enum class MineralName {
    NAME_UNDEFINED,
    HALKOPIRIT,
    BORNIT,
    HALKOZIN,
    PIRIT,
    MARKAZIT,
    SFALERIT,
    GALENIT,
    CINABARIT,
    WULFENIT,
    BARIT,
    SREBRONOSNI_GALENIT,
    SAMORODNO_ZIVO_SREBRO,
    ANTIMONIT,
    SIDERIT,
    HEMATIT,
    MAGNETIT,
    LIMONIT,
    PSILOMELAN,
    PIROLUZIT,
    URANOVA_SMOLA,
    COFFINIT,
    BOKSIT,
    SAMORODNO_ZLATO,
    TENNANTIT,
    MALAHIT,
    KUPRIT,
    AZURIT,
    REALGAR,
    TORIJEVI_MINERALI,
    CLAY,
    GRAVEL,
    LIMESTONE,
    GRANITE,
    MARBLE,
    BASALT,
    DIORITE,
    QUARTZ,
    SLATE,
    PEGMATITE,
    GYPSUM,
    HALITE,
    SULFUR,
    FLUORITE,
    KAOLIN,
    FELDSPAR,
    MICA,
    CALCITE,
    DOLOMITE,
    SERPENTINE,
    CHLORITE,
    KYANITE,
    CORUNDUM,
    EMERALD,
    RUBY,
    SAPPHIRE,
    OPAL,
    TURQUOISE,
    GRAPHITE,
    PLAGIOCLASE,
    ANDALUSITE,
    SCAPOLITE,
    TOURMALINE,
    OLIVINE
};

class MineralModel
{
private:
    MineralName name;
    double min;
    double max;
    MineralGrade grade;
public:
    MineralModel(MineralName name, double min, double max, MineralGrade grade);
    MineralModel();
    
    void getFromBsonDocument(const bsoncxx::document::view& docView);
    std::string extractStringFromBSON(const bsoncxx::document::view& docView, const char* key);
    double extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key);
    int extractIntFromBSON(const bsoncxx::document::view &docView, const char *key);
    bsoncxx::document::value convertToBsonDocument();

    bool validateMinerals() const;

    static std::string mineralGradeToString(int grade);
    static std::string mineralNameToString(int name);
};
#endif