//
// Created by Anei Markovič on 25. 5. 25.
//
#include "Model/PointModel.hpp"
PointModel::PointModel(double lat, double lon) : lat(lat), lon(lon){};
PointModel::PointModel() : lat(0), lon(0) {};
double PointModel::getLat(){
    return this->lat;
}
double PointModel::getLon(){
    return this->lon;
}
std::string PointModel::toString() {
    return "[" + std::to_string(lat) + "," + std::to_string(lon) + "]";
}

void PointModel::getFromBsonDocument(const bsoncxx::document::view& docView){
    try
    {
        this->lat = extractDoubleFromBSON(docView, "lat");
        this->lon = extractDoubleFromBSON(docView, "lng");
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for MineralModel: " << exception.what() << std::endl;
    }
}

double PointModel::extractDoubleFromBSON(const bsoncxx::document::view &docView, const char *key){
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

bsoncxx::document::value PointModel::convertToBsonDocument(){
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("lat", this->lat));
    builder.append(bsoncxx::builder::basic::kvp("lng", this->lon));
    return builder.extract();
}