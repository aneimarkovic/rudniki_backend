//  Created by Anei Markovič 22.5.2025
#include "Model/MineModel.hpp"

MineModel::MineModel(std::string name, bsoncxx::oid ownerId, MineStatus status, MineType type, std::vector<MineralModel> minerals, std::vector<InfrastructureModel> infrastructure, std::vector<WorkerModel> workers, timeStamp createdAt, timeStamp modifiedAt,  std::string municipality, int startYear, int endYear, double lon, double lat)
    : name(name),
      ownerId(ownerId),
      status(status),
      type(type),
      minerals(minerals),
      infrastructure(infrastructure),
      workers(workers),
      startYear(startYear),
      endYear(endYear),
      municipality(municipality),
      lat(lat),
      lon(lon),
      ModelTemplate(createdAt, modifiedAt)
{
    // validateMineData();
}

MineModel::MineModel()
    : ModelTemplate(GET_NOW_IN_MILLISECONDS(), GET_NOW_IN_MILLISECONDS())
{
    this->name = "";
    this->ownerId = bsoncxx::oid();
    this->status = MineStatus::CLOSED;
    this->type = MineType::SURFACE;

    std::chrono::time_point<std::chrono::system_clock,
            std::chrono::system_clock::duration>
            now = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock, std::chrono::days> today =
            std::chrono::time_point_cast<std::chrono::days>(now);
    std::chrono::year_month_day ymd = std::chrono::year_month_day(today);
    std::chrono::year year = ymd.year();

    this->startYear = static_cast<int>(year);
    this->endYear = static_cast<int>(year);
    this->municipality = "";
    this->lat = 0.00;
    this->lon = 0.00;
    this->minerals = {};
    this->infrastructure = {};
    this->workers = {};
}
void MineModel::setLon(double newLon){
    this->lon = newLon;
}
void MineModel::setLat(double newLat){
    this->lat = newLat;
}
/*
    Funkcija zgradi MineModel objekt iz pridobljenega BSON dokumenta
*/
void MineModel::getFromBsonDocument(const bsoncxx::document::view &docView)
{
    try
    {
        auto tempID = docView["_id"];
        if (tempID && tempID.type() == bsoncxx::type::k_oid)
        {
            this->id = docView["_id"].get_oid().value;
        }
        else
        {
            this->id = bsoncxx::oid();
        }
        this->name = extractStringFromBSON(docView, "name");


        // this->ownerId = docView["ownerId"].get_oid().value;

        auto ownerIdElement = docView["ownerId"];
        if (ownerIdElement && ownerIdElement.type() == bsoncxx::type::k_oid)
        {
            this->ownerId = ownerIdElement.get_oid().value;
        }
        else if (ownerIdElement && ownerIdElement.type() == bsoncxx::type::k_string)
        {
            try
            {
                auto sv = ownerIdElement.get_string().value;
                std::string str_val(sv.data(), sv.size());
                this->ownerId = bsoncxx::oid(str_val);
            }
            catch (...)
            {
                this->ownerId = bsoncxx::oid();
            }
        }
        else
        {
            this->ownerId = bsoncxx::oid();
        }

        this->status = static_cast<MineStatus>(extractIntFromBSON(docView, "status"));
        this->type = static_cast<MineType>(extractIntFromBSON(docView, "type"));

        bsoncxx::document::element tempMunicipality = docView["municipality"];
        if(tempMunicipality && tempMunicipality.type() == bsoncxx::type::k_string) {
            this->municipality = extractStringFromBSON(docView, "municipality");
        }

        bsoncxx::document::element tempYear = docView["startYear"];
        if(tempYear && tempYear.type() == bsoncxx::type::k_int32){
            this->startYear = extractIntFromBSON(docView, "startYear");
        }

        tempYear = docView["endYear"];
        if(tempYear && tempYear.type() == bsoncxx::type::k_int32){
            this->endYear = extractIntFromBSON(docView, "endYear");
        }

        bsoncxx::document::element tempLon = docView["lon"];
        if(tempLon && tempLon.type() == bsoncxx::type::k_double){
            this->lon = extractIntFromBSON(docView, "lon");
        }

        bsoncxx::document::element tempLat = docView["lat"];
        if(tempLat && tempLat.type() == bsoncxx::type::k_double){
            this->lat = extractIntFromBSON(docView, "lat");
        }

        bsoncxx::array::view tempMinerals = docView["minerals"].get_array().value;
        for (const auto &item : tempMinerals)
        {
            MineralModel temp;
            temp.getFromBsonDocument(item.get_document().view());
            this->minerals.push_back(temp);
        }

        bsoncxx::array::view tempInfrastructure = docView["infrastructure"].get_array().value;
        for (const auto &item : tempInfrastructure)
        {
            InfrastructureModel temp;
            temp.getFromBsonDocument(item.get_document().view());
            this->infrastructure.push_back(temp);
        }

        bsoncxx::array::view tempWorkers = docView["workers"].get_array().value;
        for (const auto &item : tempWorkers)
        {
            WorkerModel temp;
            temp.getFromBsonDocument(item.get_document().view());
            this->workers.push_back(temp);
        }
    }
    catch (const bsoncxx::exception &exception)
    {
        std::cerr << "BSON Deserialization Error for MineModel: " << exception.what() << std::endl;
    }
}
/*
    Funkcija zgradi BSON dokument iz trenutnega MineModel objekta
*/
bsoncxx::document::value MineModel::convertToBsonDocument()
{
    bsoncxx::builder::basic::document builder{};
    builder.append(bsoncxx::builder::basic::kvp("_id", id));
    builder.append(bsoncxx::builder::basic::kvp("name", this->name));
    builder.append(bsoncxx::builder::basic::kvp("startYear", this->startYear));
    builder.append(bsoncxx::builder::basic::kvp("endYear", this->endYear));
    builder.append(bsoncxx::builder::basic::kvp("municipality", this->municipality));
    builder.append(bsoncxx::builder::basic::kvp("lon", this->lon));
    builder.append(bsoncxx::builder::basic::kvp("lat", this->lat));
    builder.append(bsoncxx::builder::basic::kvp("ownerId", this->ownerId));
    builder.append(bsoncxx::builder::basic::kvp("status", static_cast<int>(this->status)));
    builder.append(bsoncxx::builder::basic::kvp("type", static_cast<int>(this->type)));
    builder.append(bsoncxx::builder::basic::kvp("created", bsoncxx::types::b_date{created}));
    builder.append(bsoncxx::builder::basic::kvp("modified", bsoncxx::types::b_date{modified}));

    auto mineralsArr = bsoncxx::builder::basic::array{};
    for (auto &item : this->minerals)
    {
        mineralsArr.append(item.convertToBsonDocument().view());
    }
    builder.append(bsoncxx::builder::basic::kvp("minerals", mineralsArr));
    auto infrastructureArr = bsoncxx::builder::basic::array{};
    for (auto &item : this->infrastructure)
    {
        infrastructureArr.append(item.convertToBsonDocument().view());
    }
    builder.append(bsoncxx::builder::basic::kvp("infrastructure", infrastructureArr));
    auto workersArr = bsoncxx::builder::basic::array{};
    for (auto &item : this->workers)
    {
        workersArr.append(item.convertToBsonDocument().view());
    }
    builder.append(bsoncxx::builder::basic::kvp("workers", workersArr));
    return builder.extract();
}
/*
    Funkcija, ki preveri oz validira podatke rudnika
*/
bool MineModel::validateMineData() const
{
    if (this->type > MineType::DEEPSEA || this->type < MineType::SURFACE)
    {
        return false;
    }

    if (this->status > MineStatus::BUILDING || this->status < MineStatus::ACTIVE)
    {
        return false;
    }

    for (MineralModel item : this->minerals)
    {
        if (!item.validateMinerals())
        {
            return false;
        }
    }

    for (InfrastructureModel item : this->infrastructure)
    {
        if (!item.validateInfrastructure())
        {
            return false;
        }
    }

    for (WorkerModel item : this->workers)
    {
        if (!item.validateWorkers())
        {
            return false;
        }
    }
    return true;
}

std::string MineModel::mineStatusToString(int status) {
    switch (status) {
    case 0: return "ACTIVE";
    case 1: return "IDLE";
    case 2: return "CLOSED";
    case 3: return "BUILDING";
    default: return "UNKNOWN_STATUS";
    }
}

std::string MineModel::mineTypeToString(int type) {
    switch (type) {
    case 0: return "SURFACE";
    case 1: return "UNDERGROUND";
    case 2: return "PLACER";
    case 3: return "INSITU";
    case 4: return "DEEPSEA";
    default: return "UNKNOWN_TYPE";
    }
}