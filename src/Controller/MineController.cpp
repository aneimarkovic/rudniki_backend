// Created by Anei Markovic 22.5.2025
#include <iostream>

#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/builder/stream/array.hpp"

#include "Controller/MineController.hpp"
#include "Model/MineModel.hpp"
#include "Model/PointModel.hpp"
#include "Model/BordersModel.hpp"
#include "DatabaseHandler.hpp"
#include "WebSocket/WebsocketController.hpp"

// Funkcija shrani novi rudnik v bazo
void MineController::saveMine(const request& request, response& response, Router* r) {
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    MineModel temp;
    temp.getFromBsonDocument(view);

    BordersModel borders;
    borders.getFromBsonDocument(view);

    PointModel tempPoint = borders.getPoints()[0];
    temp.setLat(tempPoint.getLat());
    temp.setLon(tempPoint.getLon());

    bsoncxx::document::value insertDocument = temp.convertToBsonDocument();

    std::optional<bsoncxx::oid> id = DatabaseHandler::insertDocumentGetInsertId("mines", insertDocument);

    if (id) {
        bsoncxx::oid actualId = *id;
        borders.setMineId(actualId);

        insertDocument = borders.convertToBsonDocument();
        bool bordersInserted = DatabaseHandler::insertDocument("bordersTest", insertDocument);

        if (bordersInserted) {
            // Get the mine document and add geometry array to it
            bsoncxx::document::value mineDoc = temp.convertToBsonDocument();
            bsoncxx::document::value bordersDoc = borders.convertToBsonDocument();

            // Create the combined document with geometry as an array
            auto builder = bsoncxx::builder::stream::document{};

            // Add all mine fields
            for (auto&& element : mineDoc.view()) {
                builder << element.key() << element.get_value();
            }

            // Add geometry as an array containing the borders document
            builder << "geometry" << bsoncxx::builder::stream::open_array
                << bordersDoc
                << bsoncxx::builder::stream::close_array;

            bsoncxx::document::value responseDoc = builder << bsoncxx::builder::stream::finalize;

            std::string responseJson = bsoncxx::to_json(responseDoc.view());

            // Set response body for Boost Beast
            response.body() = responseJson;
            response.set(boost::beast::http::field::content_type, "application/json");
            response.prepare_payload();

            // Send via WebSocket
            WebSocketController::sendBroadcast("rudnikSubscribe", responseDoc);
        }
        else {
            // Handle borders insertion failure
            bsoncxx::document::value errorDoc = bsoncxx::builder::stream::document{}
                << "success" << false
                << "message" << "Pri vstavlajnju mej je prišlo do napake!"
                << bsoncxx::builder::stream::finalize;

            std::string errorJson = bsoncxx::to_json(errorDoc.view());
            response.body() = errorJson;
            response.set(boost::beast::http::field::content_type, "application/json");
            response.prepare_payload();
        }
    }
    else {
        // Handle mine insertion failure
        bsoncxx::document::value errorDoc = bsoncxx::builder::stream::document{}
            << "success" << false
            << "message" << "Pri vstavlajnju rudnika je prišlo do napake!"
            << bsoncxx::builder::stream::finalize;

        std::string errorJson = bsoncxx::to_json(errorDoc.view());
        response.body() = errorJson;
        response.set(boost::beast::http::field::content_type, "application/json");
        response.prepare_payload();
    }
}

// Funkcija za pridobivanje rudnika
void MineController::getMine(const request &request, response &response, Router *r)
{
  auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{r->UrlArguments[0]}));

  std::optional<bsoncxx::document::value> mineDoc = DatabaseHandler::fetchSingleDocument("mines", filters);
  if (mineDoc)
  {
    bsoncxx::document::view viewTemp = mineDoc->view();
    std::string jsonStr = bsoncxx::to_json(viewTemp);
    response.body() = jsonStr;
  }
  else
  {
    std::cout << "Neobstaja\n";
  }
}

// Funkcija za brisanje rudnika
void MineController::deleteMine(const request &request, response &response, Router *r)
{
  auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{r->UrlArguments[0]}));

  std::string resString = (DatabaseHandler::deleteDocument("mines", filters) == true ? ("Rudnik uspešno izbrisan!") : ("Pri brisanju rudnika je prišlo do napake!"));
  bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
  bsoncxx::document::view viewTemp = documentTemp.view();
  std::string jsonStr = bsoncxx::to_json(viewTemp);

  response.body() = jsonStr;
}

// Funkcija za dodajanje infrastrukture v že obstoječi rudnik
void MineController::addInfrastructure(const request &request, response &response, Router *r)
{
  bsoncxx::document::value document = bsoncxx::from_json(request.body());
  bsoncxx::document::view view = document.view();

  auto mineIdElement = view["id"];
  auto stringView = mineIdElement.get_string().value;
  std::string str_val(stringView.data(), stringView.size());
  bsoncxx::oid mineID = bsoncxx::oid(str_val);

  auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", mineID));

  std::vector<InfrastructureModel> infrastructureArr;
  bsoncxx::array::view tempInfrastructure = view["infrastructure"].get_array().value;
  for (const auto &item : tempInfrastructure)
  {
    InfrastructureModel temp;
    temp.getFromBsonDocument(item.get_document().view());
    infrastructureArr.push_back(temp);
  }
  auto infrastructureArrBson = bsoncxx::builder::basic::array{};
  for (auto &item : infrastructureArr)
  {
    infrastructureArrBson.append(item.convertToBsonDocument().view());
  }

  bsoncxx::builder::basic::document updateDoc;
  updateDoc.append(
      bsoncxx::builder::basic::kvp("$push",
                                   bsoncxx::builder::basic::make_document(
                                       bsoncxx::builder::basic::kvp("infrastructure",
                                                                    bsoncxx::builder::basic::make_document(
                                                                        bsoncxx::builder::basic::kvp("$each", infrastructureArrBson))))));

  bsoncxx::document::value updateValue = updateDoc.extract();
  std::string resString = (DatabaseHandler::updateOneItem("mines", filters, updateValue) == true ? ("Infrastruktura uspešno dodana!") : ("Pri dodajanju infrastrukture je prišlo do napake!"));
  bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
  response.body() = bsoncxx::to_json(documentTemp.view());
}

// Funkcija za dodajanje mineralov v že obstoječi rudnik
void MineController::addMineral(const request &request, response &response, Router *r)
{
  bsoncxx::document::value document = bsoncxx::from_json(request.body());
  bsoncxx::document::view view = document.view();

  auto mineIdElement = view["id"];
  auto stringView = mineIdElement.get_string().value;
  std::string str_val(stringView.data(), stringView.size());
  bsoncxx::oid mineID = bsoncxx::oid(str_val);

  auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", mineID));

  std::vector<MineralModel> mineralArr;
  bsoncxx::array::view tempMineral = view["minerals"].get_array().value;
  for (const auto &item : tempMineral)
  {
    MineralModel temp;
    temp.getFromBsonDocument(item.get_document().view());
    mineralArr.push_back(temp);
  }
  auto mineralArrBson = bsoncxx::builder::basic::array{};
  for (auto &item : mineralArr)
  {
    mineralArrBson.append(item.convertToBsonDocument().view());
  }

  bsoncxx::builder::basic::document updateDoc;
  updateDoc.append(
      bsoncxx::builder::basic::kvp("$push",
                                   bsoncxx::builder::basic::make_document(
                                       bsoncxx::builder::basic::kvp("minerals",
                                                                    bsoncxx::builder::basic::make_document(
                                                                        bsoncxx::builder::basic::kvp("$each", mineralArrBson))))));

  bsoncxx::document::value updateValue = updateDoc.extract();
  std::string resString = (DatabaseHandler::updateOneItem("mines", filters, updateValue) == true ? ("Minerali uspešno dodani!") : ("Pri dodajanju mineralov je prišlo do napake!"));
  bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
  response.body() = bsoncxx::to_json(documentTemp.view());
}

// Funkcija za dodajanje delavcov v že obstoječi rudnik
void MineController::addWorker(const request &request, response &response, Router *r)
{
  bsoncxx::document::value document = bsoncxx::from_json(request.body());
  bsoncxx::document::view view = document.view();

  auto mineIdElement = view["id"];
  auto stringView = mineIdElement.get_string().value;
  std::string str_val(stringView.data(), stringView.size());
  bsoncxx::oid mineID = bsoncxx::oid(str_val);

  auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", mineID));

  std::vector<WorkerModel> workerArr;
  bsoncxx::array::view tempMineral = view["workers"].get_array().value;
  for (const auto &item : tempMineral)
  {
    WorkerModel temp;
    temp.getFromBsonDocument(item.get_document().view());
    workerArr.push_back(temp);
  }
  auto workersArrBson = bsoncxx::builder::basic::array{};
  for (auto &item : workerArr)
  {
    workersArrBson.append(item.convertToBsonDocument().view());
  }

  bsoncxx::builder::basic::document updateDoc;
  updateDoc.append(
      bsoncxx::builder::basic::kvp("$push",
                                   bsoncxx::builder::basic::make_document(
                                       bsoncxx::builder::basic::kvp("workers",
                                                                    bsoncxx::builder::basic::make_document(
                                                                        bsoncxx::builder::basic::kvp("$each", workersArrBson))))));

  bsoncxx::document::value updateValue = updateDoc.extract();
  std::string resString = (DatabaseHandler::updateOneItem("mines", filters, updateValue) == true ? ("Delavci uspešno dodani!") : ("Pri dodajanju delavcev je prišlo do napake!"));
  bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
  response.body() = bsoncxx::to_json(documentTemp.view());
}

//Funkcija, ki pridobi vse razpoložljive minerale
void MineController::generateMineralsValue(const request &request, response &response, Router *r)
{
  bsoncxx::document::value document = bsoncxx::from_json(request.body());
  bsoncxx::document::view view = document.view();

  double pi = 3.14159200000000016217427400988526642322540283203125;
  double earthRadius = 6378000.0;

  PointModel center(view["lon"].get_double(), view["lat"].get_double());

  double distance = 5000; // 5000m

  double lat1 = center.getLat() * pi / 180.0;
  double lon1 = center.getLon() * pi / 180.0;

  std::vector<PointModel> pointVec;
  for (int i = 0; i < 360; i += 2)
  {
    double angle = i * pi / 180.0;

    double lat = std::asin(std::sin(lat1) * std::cos(distance / earthRadius) +
                           std::cos(lat1) * std::sin(distance / earthRadius) * std::cos(angle));

    double lon = lon1 + std::atan2(std::sin(angle) * std::sin(distance / earthRadius) * std::cos(lat1),
                                   std::cos(distance / earthRadius) - std::sin(lat1) * std::sin(lat));

    PointModel temp(lat * 180.0 / pi, lon * 180.0 / pi);
    pointVec.push_back(temp);
  }

  // Da sklenemo krog
  pointVec.push_back(pointVec[0]);

  bsoncxx::builder::basic::array coordinatesArray = bsoncxx::builder::basic::array{};
  for (PointModel it : pointVec)
  {
    bsoncxx::builder::basic::array tempArr = bsoncxx::builder::basic::array{};
    tempArr.append(it.getLon());
    tempArr.append(it.getLat());
    coordinatesArray.append(tempArr.extract());
  }


  bsoncxx::builder::basic::array polygon;
  polygon.append(coordinatesArray.extract());

    bsoncxx::builder::basic::document polygonDoc;
    polygonDoc.append(
            bsoncxx::builder::basic::kvp("type", "Polygon"),
            bsoncxx::builder::basic::kvp("coordinates", polygon)
    );
    bsoncxx::builder::basic::document geoWithinDoc;
    geoWithinDoc.append(
            bsoncxx::builder::basic::kvp("$geometry", polygonDoc)
    );
    bsoncxx::builder::basic::document query;
    query.append(
    bsoncxx::builder::basic::kvp("geometry",
                                 bsoncxx::builder::basic::make_document(
                                         bsoncxx::builder::basic::kvp("$geoWithin", geoWithinDoc)
                                 )
    )
    );

//    std::cout <<  bsoncxx::to_json(query.view()) << std::endl;

    std::vector<bsoncxx::document::value>
          results = DatabaseHandler::fetchMultipleDocuments("bordersTest", query.view());

  std::vector<bsoncxx::oid> vecOfMines;
  for (bsoncxx::document::value& it : results)
  {
//      std::cout << bsoncxx::to_json(it) << std::endl;
      view = it.view();
      if (view["mineId"] && view["mineId"].type() == bsoncxx::type::k_oid)
      {
          bsoncxx::oid id = view["mineId"].get_oid().value;
          vecOfMines.push_back(id);
      }
  }

    bsoncxx::builder::stream::document filterBuilder;
    bsoncxx::builder::stream::document filterBuilderHistory;

    auto arrayBuilder = bsoncxx::builder::stream::array{};
    auto arrayBuilderHistory = bsoncxx::builder::stream::array{};
    for (const bsoncxx::oid& oid : vecOfMines)
    {
//        std::cout << oid.to_string() << std::endl;
        arrayBuilder << oid;
        arrayBuilderHistory << oid;
    }

    mongocxx::pipeline pipeline{};

    pipeline.match(
            bsoncxx::builder::stream::document{}
                    << "mineId" << bsoncxx::builder::stream::open_document
                    << "$in" << arrayBuilderHistory
                    << bsoncxx::builder::stream::close_document
                    << bsoncxx::builder::stream::finalize
    );

    pipeline.unwind("$minerals");

    pipeline.group(
            bsoncxx::builder::stream::document{}
                    << "_id" << bsoncxx::builder::stream::open_document
                    << "mineId" << "$mineId"
                    << "mineralName" << "$minerals.name"
                    << bsoncxx::builder::stream::close_document
                    << "avgQuantity" << bsoncxx::builder::stream::open_document
                    << "$avg" << "$minerals.quantity"
                    << bsoncxx::builder::stream::close_document
                    << bsoncxx::builder::stream::finalize
    );

    pipeline.project(
            bsoncxx::builder::stream::document{}
                    << "_id" << 0
//                    << "mineId" << "$_id.mineId"
                    << "mineralName" << "$_id.mineralName"
                    << "avgQuantity" << 1
                    << bsoncxx::builder::stream::finalize
    );

    struct AvgHistory{
        int mineralName;
        double avgQuantity;
        int counter = 1;
    };

    std::vector<bsoncxx::document::value> historyResult = DatabaseHandler::fetchMultipleDocumentsAggregate("history", pipeline);

    std::vector<AvgHistory> vecOfAvgHistory;
    for(bsoncxx::document::value& it : historyResult){
        view = it.view();

        AvgHistory temp;
        temp.mineralName = view["mineralName"].get_int32();
        temp.avgQuantity = view["avgQuantity"].get_double();
        vecOfAvgHistory.push_back(temp);
        std::cout << bsoncxx::to_json(it) << std::endl;
    }

    std::vector<AvgHistory> vecOfAllHistory;
    for(AvgHistory & it : vecOfAvgHistory){
        bool found = false;
        int index = 0;
        for(int j = 0; j < vecOfAllHistory.size(); j++){
            if(it.mineralName == vecOfAllHistory[j].mineralName){
                found = true;
                index = j;
            }
        }

        if(!found){
            vecOfAllHistory.push_back(it);
        } else{
            vecOfAllHistory[index].avgQuantity += it.avgQuantity;
            vecOfAllHistory[index].counter++;
        }
    }

    filterBuilder << "_id" << bsoncxx::builder::stream::open_document
                   << "$in" << arrayBuilder
                   << bsoncxx::builder::stream::close_document;

    bsoncxx::document::value filterDoc = filterBuilder << bsoncxx::builder::stream::finalize;
    mongocxx::options::find opts{};
    opts.projection(bsoncxx::builder::stream::document{} << "minerals" << 1 << "_id" << 0 << bsoncxx::builder::stream::finalize);

    std::vector<bsoncxx::document::value> mineralsDoc = DatabaseHandler::getSpecificColumnFromDocument(
            "mines", opts, std::move(filterDoc)
    );


    std::string temp = "{";

    temp += "\"minerals\":[";
    int counter = 0;
    for (bsoncxx::document::value& it : mineralsDoc) {
        temp += bsoncxx::to_json(it);
        if (counter < mineralsDoc.size() - 1) {
            temp += ",";
        }
        counter++;
    }
    temp += "],";

    temp += "\"history\":[";
    counter = 0;
    for (AvgHistory &it : vecOfAllHistory) {
        it.avgQuantity /= (it.counter * 150);

        temp += "{";
        temp += "\"mineralName\":" + std::to_string(it.mineralName) + ", ";
        temp += "\"avgQuantity\":" + std::to_string(it.avgQuantity);
        temp += "}";
        if (counter < vecOfAllHistory.size() - 1) {
            temp += ",";
        }
        counter++;
    }
    temp += "]";

    temp += "}";

    response.body() += temp;
}
//Funkcija, ki vrne vse razpoložljive rudnika pridobljene z scraperom
void MineController::getScrapperMines(const request &request, response &response, Router* r){
     bsoncxx::document::view filters{};
     std::string collName = "mines";
     std::vector<bsoncxx::document::value> scrapperVec = DatabaseHandler::fetchMultipleDocuments(collName, filters);

     std::string temp = "{";
     int counter = 0;
     for (auto &&i : scrapperVec)
     {
         // std::cout << bsoncxx::to_json(i) << std::endl;
         temp += "\"" + std::to_string(counter) + "\":" + bsoncxx::to_json(i);
         if(counter < scrapperVec.size()-1){
             temp += ",";
         }
         counter++;
     }

     temp += "}";
    response.body() = temp;
}

void MineController::getFilteredMines(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    bsoncxx::builder::stream::document query;

    bsoncxx::document::element mineType = view["type"];
    if(mineType && mineType.type() != bsoncxx::type::k_null){
        query << "type" << mineType.get_int32();
    }

    bsoncxx::document::element mineStatus = view["status"];
    if(mineStatus && mineStatus.type() != bsoncxx::type::k_null){
        query << "status" << mineStatus.get_int32();
    }

    bsoncxx::document::element minerals = view["minerals"];

    if(minerals && minerals.type() != bsoncxx::type::k_array){
        bsoncxx::document::view mineralsArr = minerals.get_array().value;

        bsoncxx::builder::stream::array queryArr;
        for(const bsoncxx::document::element& mineralElement : mineralsArr){
            bsoncxx::document::view mineral = mineralElement.get_document().view();

            bsoncxx::builder::stream::document mineralsQuery; //Znotraj glovnega

            bsoncxx::document::element mineralName = view["name"];
            if(mineralName.type() == bsoncxx::type::k_int32){
                mineralsQuery << "name" << mineralName.get_int32();
            }

            bsoncxx::document::element minLowerBound = view["minLowerBound"];
            bsoncxx::document::element minUpperBound = view["minUpperBound"];
            if(minLowerBound.type() == bsoncxx::type::k_double || minUpperBound.type() == bsoncxx::type::k_double){
                bsoncxx::builder::stream::document boundsDocument; //Za min pa max
                if(minLowerBound && minLowerBound.type() != bsoncxx::type::k_null){
                    boundsDocument << "$gte" << minLowerBound.get_double();
                }
                if(minUpperBound && minUpperBound.type() != bsoncxx::type::k_null){
                    boundsDocument << "$lte" << minUpperBound.get_double();
                }

                mineralsQuery << "min" << boundsDocument;
            }

            bsoncxx::document::element maxLowerBound = view["maxLowerBound"];
            bsoncxx::document::element maxUpperBound = view["maxUpperBound"];
            if(maxLowerBound.type() == bsoncxx::type::k_double || maxUpperBound.type() == bsoncxx::type::k_double){
                bsoncxx::builder::stream::document boundsDocument; //Za min pa max
                if(maxLowerBound && maxLowerBound.type() != bsoncxx::type::k_null){
                    boundsDocument << "$gte" << maxLowerBound.get_double();
                }
                if(maxUpperBound && maxUpperBound.type() != bsoncxx::type::k_null){
                    boundsDocument << "$lte" << maxUpperBound.get_double();
                }

                mineralsQuery << "max" << boundsDocument;
            }
            bsoncxx::document::element mineralGrade = view["grade"];
            if(mineralGrade.type() == bsoncxx::type::k_int32){
                mineralsQuery << "grade" << mineralGrade.get_int32();
            }

            //Če ni empty add
            if(!mineralsQuery.view().empty()){
                bsoncxx::builder::stream::document mineralsDocument;
                mineralsDocument << "minerals" << bsoncxx::builder::stream::open_document << "$elemMatch" << mineralsQuery <<  bsoncxx::builder::stream::close_document;
                queryArr << mineralsDocument;
            }
        }

        if(!queryArr.view().empty()){
            query << "$or" << queryArr;
        }
    }

    std::vector<bsoncxx::document::value>
            results = DatabaseHandler::fetchMultipleDocuments("mines", query.view());

    std::string temp = "{";
    int counter = 0;
    for (bsoncxx::document::value& it : results)
    {
//      std::cout << bsoncxx::to_json(it) << std::endl;
        temp += "\"" + std::to_string(counter) + "\":" + bsoncxx::to_json(it);
        if(counter < results.size() - 1){
            temp += ",";
        }
        counter++;
    }
    temp += "}";
    response.body() += temp;
}
void MineController::getMineBasedOnOwner(const request &request, response &response, Router *r){
    auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ownerId", bsoncxx::oid{r->UrlArguments[0]}));

    std::vector<bsoncxx::document::value> result = DatabaseHandler::fetchMultipleDocuments("mines", filters);
    if (!result.empty())
    {
        std::string temp = "{";
        int counter = 0;
        for (bsoncxx::document::value& it : result)
        {
//      std::cout << bsoncxx::to_json(it) << std::endl;
            temp += "\"" + std::to_string(counter) + "\":" + bsoncxx::to_json(it);
            if(counter < result.size() - 1){
                temp += ",";
            }
            counter++;
        }
        temp += "}";
        response.body() += temp;
    }
    else
    {
        std::cout << "Neobstaja\n";
    }
}

void MineController::getAllMines(const request &request, response &response, Router* r){
    mongocxx::pipeline pipeline;
    pipeline.lookup(
            bsoncxx::builder::stream::document{}
                    << "from" << "bordersTest"
                    << "localField" << "_id"
                    << "foreignField" << "mineId"
                    << "as" << "geometry"
                    << bsoncxx::builder::stream::finalize
    );

    std::vector<bsoncxx::document::value> result = DatabaseHandler::fetchMultipleDocumentsAggregate("mines", pipeline);
    std::string temp = "{";
    int counter = 0;
    for (bsoncxx::document::value& it : result)
    {
//      std::cout << bsoncxx::to_json(it) << std::endl;
        temp += "\"" + std::to_string(counter) + "\":" + bsoncxx::to_json(it);
        if(counter < result.size() - 1){
            temp += ",";
        }
        counter++;
    }
    temp += "}";
    response.body() += temp;
}

void MineController::searchBar(const request &request, response &response, Router* r){
    bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
    builder << "name"
    << bsoncxx::builder::stream::open_document
    << "$regex" << r->UrlArguments[0] << "$options" << "i"
    << bsoncxx::builder::stream::close_document;

    bsoncxx::document::value filters = builder.extract();

//    std::cout << bsoncxx::to_json(filters.view()) << std::endl;

    std::vector<bsoncxx::document::value> result =  DatabaseHandler::fetchMultipleDocuments("mines", filters.view());

    if(!result.empty()){
        std::string temp = "{";
        int counter = 0;
        for (bsoncxx::document::value& it : result)
        {
//      std::cout << bsoncxx::to_json(it) << std::endl;
            temp += "\"" + std::to_string(counter) + "\":" + bsoncxx::to_json(it);
            if(counter < result.size() - 1){
                temp += ",";
            }
            counter++;
        }
        temp += "}";
        response.body() += temp;
    } else{
        response.body() = "Ni takih rudnikov!";
    }
}

void MineController::getMinesByYear(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    bsoncxx::builder::stream::document query;
    bsoncxx::document::element from = view["from"];
    bsoncxx::document::element to = view["to"];

    query << "startYear" << bsoncxx::builder::stream::open_document
          << "$gte" << from.get_int32().value
          << "$lte" << to.get_int32().value
          << bsoncxx::builder::stream::close_document;

    std::vector<bsoncxx::document::value> result =  DatabaseHandler::fetchMultipleDocuments("mines", query.view());

    if(!result.empty()){
        std::string temp = "{";
        int counter = 0;
        for (bsoncxx::document::value& it : result)
        {
            temp += "\"" + std::to_string(counter) + "\":" + bsoncxx::to_json(it);
            if(counter < result.size() - 1){
                temp += ",";
            }
            counter++;
        }
        temp += "}";
        response.body() += temp;
    } else{
        response.body() = "{\"message\": \"Ni rudnikov v tem časovnem intervalu!\"}";
    }
}

void MineController::deleteWorker(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    bsoncxx::builder::stream::document query;
    bsoncxx::document::element id = view["id"];
    auto stringView = id.get_string().value;
    std::string str_val(stringView.data(), stringView.size());
    bsoncxx::oid mineID = bsoncxx::oid(str_val);
    query << "_id" << mineID;

    bsoncxx::builder::stream::array insideArr;
    bsoncxx::document::element workers = view["workers"];
    bsoncxx::document::view workersArr = workers.get_array().value;
    for(const bsoncxx::document::element& worker : workersArr){
        bsoncxx::document::view workerView = worker.get_document().value;
        bsoncxx::document::element workerId = workerView["IDNumber"];
        insideArr << workerId.get_int32().value;
    }

    bsoncxx::builder::stream::document insideQuery;
    insideQuery
    << "$pull" << bsoncxx::builder::stream::open_document
    << "workers" << bsoncxx::builder::stream::open_document
    << "IDNumber" << bsoncxx::builder::stream::open_document
    << "$in" << insideArr
    << bsoncxx::builder::stream::close_document
    << bsoncxx::builder::stream::close_document
    << bsoncxx::builder::stream::close_document;

    std::string resString = (DatabaseHandler::updateOneItem("mines", query, insideQuery) ? ("Delavci uspešno odstranjeni!") : ("Pri odstranjevanju delavcov je prišlo do napake!"));
    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
    response.body() = bsoncxx::to_json(documentTemp.view());
}

void MineController::deleteInfrastructure(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    bsoncxx::builder::stream::document query;
    bsoncxx::document::element id = view["id"];
    auto stringView = id.get_string().value;
    std::string str_val(stringView.data(), stringView.size());
    bsoncxx::oid mineID = bsoncxx::oid(str_val);
    query << "_id" << mineID;

    bsoncxx::builder::stream::array insideArr;
    bsoncxx::document::element infrastructure = view["infrastructure"];
    bsoncxx::document::view infrastructureArr = infrastructure.get_array().value;
    for(const bsoncxx::document::element& worker : infrastructureArr){
        bsoncxx::document::view viewTemp = worker.get_document().value;
        bsoncxx::document::element infrastructureId = viewTemp["IDNumber"];
        insideArr << infrastructureId.get_int32().value;
    }

    bsoncxx::builder::stream::document insideQuery;
    insideQuery
            << "$pull" << bsoncxx::builder::stream::open_document
            << "infrastructure" << bsoncxx::builder::stream::open_document
            << "IDNumber" << bsoncxx::builder::stream::open_document
            << "$in" << insideArr
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_document;

    std::string resString = (DatabaseHandler::updateOneItem("mines", query, insideQuery) ? ("Infrastruktura uspešno odstranjena!") : ("Pri odstranjevanju infrastrukture je prišlo do napake!"));
    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
    response.body() = bsoncxx::to_json(documentTemp.view());
}

void MineController::deleteMineral(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    bsoncxx::builder::stream::document query;
    bsoncxx::document::element id = view["id"];
    auto stringView = id.get_string().value;
    std::string str_val(stringView.data(), stringView.size());
    bsoncxx::oid mineID = bsoncxx::oid(str_val);
    query << "_id" << mineID;

    bsoncxx::builder::stream::array insideArr;
    bsoncxx::document::element mineral = view["minerals"];
    bsoncxx::document::view mineralArr = mineral.get_array().value;
    for(const bsoncxx::document::element& worker : mineralArr){
        bsoncxx::document::view viewTemp = worker.get_document().value;
        bsoncxx::document::element mineralName = viewTemp["name"];
        insideArr << mineralName.get_int32().value;
    }

    bsoncxx::builder::stream::document insideQuery;
    insideQuery
            << "$pull" << bsoncxx::builder::stream::open_document
            << "minerals" << bsoncxx::builder::stream::open_document
            << "name" << bsoncxx::builder::stream::open_document
            << "$in" << insideArr
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_document;

    std::string resString = (DatabaseHandler::updateOneItem("mines", query, insideQuery) ? ("Minerali uspešno odstranjeni!") : ("Pri odstranjevanju mineralov je prišlo do napake!"));
    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
    response.body() = bsoncxx::to_json(documentTemp.view());
}

void MineController::updateMine(const request& request, response& response, Router* r) {
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    std::string token = "";
    auto it = request.find(boost::beast::http::field::cookie);
    if (it != request.end())
    {
        auto cookie_header = std::string(it->value());
        size_t pos = cookie_header.find("jwt=");
        if (pos != std::string::npos) {
            size_t start = pos + 4;
            size_t end = cookie_header.find(";", start);
            token = cookie_header.substr(start, end - start);
        }
    }

    bsoncxx::builder::stream::document query;
    bsoncxx::document::element id = view["id"];
    auto stringView = id.get_string().value;
    std::string str_val(stringView.data(), stringView.size());
    bsoncxx::oid mineID = bsoncxx::oid(str_val);


    mongocxx::options::find opts{};
    opts.projection(bsoncxx::builder::stream::document{} << "ownerId" << 1 << "_id" << 0 << bsoncxx::builder::stream::finalize);

    bsoncxx::builder::stream::document filters;
    filters << "_id" << mineID;
    bsoncxx::document::value docValue = filters << bsoncxx::builder::stream::finalize;
    std::vector<bsoncxx::document::value> result = DatabaseHandler::getSpecificColumnFromDocument("users", opts, docValue);

    if (result.size() == 0) {
        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << "Napaka ob posodabljanju rudnika lastnik rudnika in prijavljeni uporabnik se ne ujemata!" << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);

        response.body() = jsonStr;
        return;
    }

    if (token.empty()) {
        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << "Potrebna prijava!" << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);

        response.body() = jsonStr;
        return;
    }

    bsoncxx::oid userId = UserModel::getUserIdFromJWT(token);

    if (userId != result[0].view()["ownerId"].get_oid().value) {
        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << "Rudnik lahko updata samo lastnik!" << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);

        response.body() = jsonStr;
        return;
    }


    query << "_id" << mineID;

    bsoncxx::builder::stream::document updateQuery;
    bsoncxx::document::element element = view["name"];
    if (element && element.type() == bsoncxx::type::k_string) {
        updateQuery << "name" << element.get_string().value;
    }

    element = view["municipality"];
    if (element && element.type() == bsoncxx::type::k_string) {
        updateQuery << "municipality" << element.get_string().value;
    }

    element = view["status"];
    if (element && element.type() == bsoncxx::type::k_int32) {
        updateQuery << "status" << element.get_int32().value;
    }

    element = view["type"];
    if (element && element.type() == bsoncxx::type::k_int32) {
        updateQuery << "type" << element.get_int32().value;
    }

    bsoncxx::builder::stream::document updateSet;
    updateSet << "$set" << updateQuery.view();

    std::string resString = (DatabaseHandler::updateOneItem("mines", query, updateSet) ? ("Rudnik uspešno posodobljen!") : ("Pri posodabljanju rudnika je prišlo do napake!"));
    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
    response.body() = bsoncxx::to_json(documentTemp.view());
}

/*
    Funkcija izračuna statistične podakte o rudnikih in jih pošlje odjemalcu
*/
void MineController::getStatistics(const request& req, response& res, Router* r)
{
    mongocxx::pipeline pipeline;

    pipeline.facet(
        bsoncxx::builder::stream::document{}
        << "totalMines" << bsoncxx::builder::stream::open_array
        << bsoncxx::builder::stream::open_document
        << "$count" << "count"
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_array

        << "minesPerMunicipality" << bsoncxx::builder::stream::open_array
        << bsoncxx::builder::stream::open_document
        << "$group" << bsoncxx::builder::stream::open_document
        << "_id" << "$municipality" 
        << "count" << bsoncxx::builder::stream::open_document << "$sum" << 1 << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_array

        << "minesPerYear" << bsoncxx::builder::stream::open_array
        << bsoncxx::builder::stream::open_document
        << "$group" << bsoncxx::builder::stream::open_document
        << "_id" << "$startYear"
        << "count" << bsoncxx::builder::stream::open_document << "$sum" << 1 << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_array

        << "minesByStatus" << bsoncxx::builder::stream::open_array
        << bsoncxx::builder::stream::open_document
        << "$group" << bsoncxx::builder::stream::open_document
        << "_id" << "$status" 
        << "count" << bsoncxx::builder::stream::open_document << "$sum" << 1 << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_array

        << "minesByMineral" << bsoncxx::builder::stream::open_array
        << bsoncxx::builder::stream::open_document
        << "$unwind" << "$minerals"
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::open_document
        << "$group" << bsoncxx::builder::stream::open_document
        << "_id" << "$minerals.name"
        << "count" << bsoncxx::builder::stream::open_document << "$sum" << 1 << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_array

        << "minesByMineralGrade" << bsoncxx::builder::stream::open_array
        << bsoncxx::builder::stream::open_document
        << "$unwind" << "$minerals"
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::open_document
        << "$group" << bsoncxx::builder::stream::open_document
        << "_id" << "$minerals.grade"
        << "count" << bsoncxx::builder::stream::open_document << "$sum" << 1 << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_array


        << "minesByType" << bsoncxx::builder::stream::open_array
        << bsoncxx::builder::stream::open_document
        << "$group" << bsoncxx::builder::stream::open_document
        << "_id" << "$type" 
        << "count" << bsoncxx::builder::stream::open_document << "$sum" << 1 << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::close_array
        << bsoncxx::builder::stream::finalize
    );


    std::vector<bsoncxx::document::value> results = DatabaseHandler::fetchMultipleDocumentsAggregate("mines", pipeline);

    bsoncxx::builder::stream::document response_builder{};

    if (!results.empty()) {
        bsoncxx::document::view facets_doc = results[0].view();

        // 1. Total Mines
        auto total_mines_array_val = facets_doc["totalMines"].get_array().value;
        if (!total_mines_array_val.empty()) {
            response_builder << "totalMines" << total_mines_array_val[0].get_document().view()["count"].get_int32().value;
        }
        else {
            response_builder << "totalMines" << 0;
        }

        // 2. Mines per Občina
        auto mpm_builder = response_builder << "minesPerMunicipality" << bsoncxx::builder::stream::open_array;
        for (const auto& item : facets_doc["minesPerMunicipality"].get_array().value) {
            bsoncxx::document::view doc = item.get_document().view();
            mpm_builder << bsoncxx::builder::stream::open_document
                << "municipality" << doc["_id"].get_string().value
                << "count" << doc["count"].get_int32().value
                << bsoncxx::builder::stream::close_document;
        }
        mpm_builder << bsoncxx::builder::stream::close_array;

        // 3. Mines per Year
        auto mpy_builder = response_builder << "minesPerYear" << bsoncxx::builder::stream::open_array;
        for (const auto& item : facets_doc["minesPerYear"].get_array().value) {
            bsoncxx::document::view doc = item.get_document().view();
            
            int32_t year = 1970;
            if (doc["_id"].type() == bsoncxx::type::k_int32) {
                year = doc["_id"].get_int32().value;
            }

            mpy_builder << bsoncxx::builder::stream::open_document
                << "startYear" << year
                << "count" << doc["count"].get_int32().value
                << bsoncxx::builder::stream::close_document;
        }
        mpy_builder << bsoncxx::builder::stream::close_array;

        // 4. Mines by Status
        auto mbs_builder = response_builder << "minesByStatus" << bsoncxx::builder::stream::open_array;
        for (const auto& item : facets_doc["minesByStatus"].get_array().value) {
            bsoncxx::document::view doc = item.get_document().view();
            std::string status_str = "UNKNOWN_DB_STATUS";

            if (doc["_id"].type() == bsoncxx::type::k_int32) {
                int32_t status_val = doc["_id"].get_int32().value;
                status_str = MineModel::mineStatusToString(status_val);
            }
            else if (doc["_id"].type() == bsoncxx::type::k_null) { 
                status_str = "NOT_SPECIFIED";
            }

            mbs_builder << bsoncxx::builder::stream::open_document
                << "status" << status_str
                << "count" << doc["count"].get_int32().value
                << bsoncxx::builder::stream::close_document;
        }
        mbs_builder << bsoncxx::builder::stream::close_array;

        // 5. Mines by Type
        auto mbt_builder = response_builder << "minesByType" << bsoncxx::builder::stream::open_array;
        for (const auto& item : facets_doc["minesByType"].get_array().value) {
            bsoncxx::document::view doc = item.get_document().view();
            std::string type_str = "UNKNOWN_DB_TYPE";

            if (doc["_id"].type() == bsoncxx::type::k_int32) {
                int32_t type_val = doc["_id"].get_int32().value;
                std::cout << "Found int32 value: " << type_val << std::endl;
                type_str = MineModel::mineTypeToString(type_val);
            }
            else if (doc["_id"].type() == bsoncxx::type::k_null) {
                std::cout << "Found null value" << std::endl;
                type_str = "NOT_SPECIFIED";
            }
            else {
                std::cout << "Unexpected type found: " << bsoncxx::to_string(doc["_id"].type()) << std::endl;
            }

            mbt_builder << bsoncxx::builder::stream::open_document
                << "type" << type_str
                << "count" << doc["count"].get_int32().value
                << bsoncxx::builder::stream::close_document;
        }
        mbt_builder << bsoncxx::builder::stream::close_array;

        // 6. Mines by Mineral
        auto mbm_builder = response_builder << "minesByMineral" << bsoncxx::builder::stream::open_array;
        for (const auto& item : facets_doc["minesByMineral"].get_array().value) {
            bsoncxx::document::view doc = item.get_document().view();
            std::string mineral_name = "UNKNOWN_MINERAL";

            if (doc["_id"].type() == bsoncxx::type::k_int32) {
                int32_t mineral_val = doc["_id"].get_int32().value;
                mineral_name = MineralModel::mineralNameToString(mineral_val);
            }

            mbm_builder << bsoncxx::builder::stream::open_document
                << "mineral" << mineral_name
                << "count" << doc["count"].get_int32().value
                << bsoncxx::builder::stream::close_document;
        }
        mbm_builder << bsoncxx::builder::stream::close_array;

        // 7. Mines by Mineral Grade
        auto mbmg_builder = response_builder << "minesByMineralGrade" << bsoncxx::builder::stream::open_array;
        for (const auto& item : facets_doc["minesByMineralGrade"].get_array().value) {
            bsoncxx::document::view doc = item.get_document().view();
            std::string grade_str = "UNKNOWN_GRADE";

            if (doc["_id"].type() == bsoncxx::type::k_int32) {
                int32_t grade_val = doc["_id"].get_int32().value;
                grade_str = MineralModel::mineralGradeToString(grade_val);
            }
            else if (doc["_id"].type() == bsoncxx::type::k_null) {
                grade_str = "NOT_SPECIFIED";
            }

            mbmg_builder << bsoncxx::builder::stream::open_document
                << "grade" << grade_str
                << "count" << doc["count"].get_int32().value
                << bsoncxx::builder::stream::close_document;
        }
        mbmg_builder << bsoncxx::builder::stream::close_array;


        std::string json_response = bsoncxx::to_json(response_builder.view());
        res.body() = json_response;
    }
    else {
        response_builder << "message" << "No statistics data found or an error occurred.";
        std::cout << "No statistics data: " << bsoncxx::to_json(response_builder.view()) << std::endl;
    }
}