// Created by Anei Markovic 22.5.2025
#include <iostream>

#include "Controller/MineController.hpp"
#include "Model/MineModel.hpp"
#include "Model/PointModel.hpp"
#include "Model/BordersModel.hpp"
#include "DatabaseHandler.hpp"

// Funkcija shrani novi rudnik v bazo
void MineController::saveMine(const request &request, response &response, Router *r)
{
  bsoncxx::document::value document = bsoncxx::from_json(request.body());
  bsoncxx::document::view view = document.view();
  MineModel temp;
  temp.getFromBsonDocument(view);
  bsoncxx::document::value insertDocument = temp.convertToBsonDocument();
//  std::string resString = (DatabaseHandler::insertDocumnter("minesTest", insertDocument) == true ? ("Rudnik uspešno vstavljen!") : ("Pri vstavlajnju rudnika je prišlo do napake!"));
  std::optional<bsoncxx::oid> id = DatabaseHandler::insertDocumentGetInsertId("minesTest", insertDocument);

  BordersModel borders;
  if(id){
    bsoncxx::oid actualId = *id;
    borders.setMineId(actualId);
  }
//  std::cout << borders.mineId.to_string() << std::endl;
  borders.getFromBsonDocument(view);
  insertDocument = borders.convertToBsonDocument();

  std::string resString = (DatabaseHandler::insertDocument("bordersTest", insertDocument) ? ("Meje uspešno vstavljen!") : ("Pri vstavlajnju mej je prišlo do napake!"));
  bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << id->to_string() << bsoncxx::builder::stream::finalize;
//  bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
  bsoncxx::document::view viewTemp = documentTemp.view();
  std::string jsonStr = bsoncxx::to_json(viewTemp);

  response.body() = jsonStr;
}

// Funkcija za pridobivanje rudnika
void MineController::getMine(const request &request, response &response, Router *r)
{
  auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{r->UrlArguments[0]}));

  std::optional<bsoncxx::document::value> mineDoc = DatabaseHandler::fetchSingleDocument("minesTest", filters);
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

  std::string resString = (DatabaseHandler::deleteDocument("minesTest", filters) == true ? ("Rudnik uspešno izbrisan!") : ("Pri brisanju rudnika je prišlo do napake!"));
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
  std::string resString = (DatabaseHandler::updateOneItem("minesTest", filters, updateValue) == true ? ("Infrastruktura uspešno dodana!") : ("Pri dodajanju infrastrukture je prišlo do napake!"));
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
  std::string resString = (DatabaseHandler::updateOneItem("minesTest", filters, updateValue) == true ? ("Minerali uspešno dodani!") : ("Pri dodajanju mineralov je prišlo do napake!"));
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
  std::string resString = (DatabaseHandler::updateOneItem("minesTest", filters, updateValue) == true ? ("Delavci uspešno dodani!") : ("Pri dodajanju delavcev je prišlo do napake!"));
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
    auto array_builder = bsoncxx::builder::stream::array{};
    for (const bsoncxx::oid& oid : vecOfMines)
    {
        array_builder << oid;
//        std::cout << "ID: " << oid.to_string() << std::endl;
    }

    filterBuilder << "_id" << bsoncxx::builder::stream::open_document
                   << "$in" << array_builder
                   << bsoncxx::builder::stream::close_document;

    bsoncxx::document::value filterDoc = filterBuilder << bsoncxx::builder::stream::finalize;
    mongocxx::options::find opts{};
    opts.projection(bsoncxx::builder::stream::document{} << "minerals" << 1 << "_id" << 0 << bsoncxx::builder::stream::finalize);

    std::vector<bsoncxx::document::value> mineralsDoc = DatabaseHandler::getSpecificColumnFromDocument(
            "minesTest", opts, std::move(filterDoc)
    );

    std::string temp;
    for(bsoncxx::document::value& it : mineralsDoc){
//        std::cout << bsoncxx::to_json(it) << std::endl;
        response.body() += bsoncxx::to_json(it);
    }

//    response.body() += temp;

  //    std::string geojson = "{ \n"
  //                          "\"type\": \"FeatureCollection\", \n"
  //                          "\"features\":[\n"
  //                          "{\n"
  //                          "\"type\": \"Feature\",\n"
  //                          "\"properties\": {},\n"
  //                          "\"geometry\":{\n"
  //                          "\"type\": \"Polygon\",\n"
  //                          "\"coordinates\":[\n"
  //                          "[\n";
  //
  //    for(int i = 0; i < pointVec.size(); i++){
  //        if(i < pointVec.size() - 1){
  //            geojson += pointVec[i].toString() + ",\n";
  //        } else {
  //            geojson += pointVec[i].toString() + "\n";
  //        }
  //    }

  //    geojson += "]\n"
  //               "]\n"
  //               "}\n"
  //               "}\n"
  //               "]\n"
  //               "}\n";

  //    response.body() = geojson;
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