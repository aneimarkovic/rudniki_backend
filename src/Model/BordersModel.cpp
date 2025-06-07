//
// Created by Anei Markovič on 26. 5. 25.
//
#include "Model/BordersModel.hpp"

BordersModel::BordersModel(bsoncxx::oid mineId, std::vector<PointModel> points) : mineId(mineId), points(points){};

BordersModel::BordersModel() : mineId(bsoncxx::oid()), points({}) {}

void BordersModel::setMineId(bsoncxx::oid newId){
    this->mineId = newId;
}

std::vector<PointModel> BordersModel::getPoints(){
    return this->points;
}
/*
    Funkcija zgradi BordersModel objekt iz pridobljenega BSON dokumenta
*/
void BordersModel::getFromBsonDocument(const bsoncxx::document::view& docView){
    auto tempID = docView["_id"];
    if (tempID && tempID.type() == bsoncxx::type::k_oid)
    {
        this->mineId = docView["_id"].get_oid().value;
    }

    bsoncxx::array::view tempGeometry = docView["geometry"].get_array().value;
    for (const auto &item : tempGeometry)
    {
        PointModel temp;
        temp.getFromBsonDocument(item.get_document().view());
        this->points.push_back(temp);
    }
}
/*
    Funkcija vzame ime polja v BSON in ga da v double format za uporabo
    Če polja ni vrne error
*/
double BordersModel::extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key){
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
    Funkcija zgradi BSON dokument iz trenutnega BordersModel objekta
*/
bsoncxx::document::value BordersModel::convertToBsonDocument(){
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("mineId", this->mineId));

    bsoncxx::builder::basic::array pointsArr = bsoncxx::builder::basic::array{};
    for (auto &item : this->points)
    {
        bsoncxx::builder::basic::array coord{};
        coord.append(item.getLon());
        coord.append(item.getLat());
        pointsArr.append(coord.extract());
    }

    bsoncxx::builder::basic::array coordinatesArray{};
    coordinatesArray.append(pointsArr.extract());

    bsoncxx::builder::basic::document geometryDocument{};
    geometryDocument.append(
            bsoncxx::builder::basic::kvp("type", "Polygon"),
            bsoncxx::builder::basic::kvp("coordinates", coordinatesArray)
    );

    builder.append(bsoncxx::builder::basic::kvp("geometry", geometryDocument.extract()));

    return builder.extract();
}