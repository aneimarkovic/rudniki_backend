// Created by Anei Markovic 22.5.2025
#include <iostream>

#include "Controller/MineController.hpp"
#include "Model/MineModel.hpp"
#include "DatabaseHandler.hpp"

// Funkcija shrani novi rudnik v bazo
void MineController::saveMine(const request &request, response &response, Router *r)
{
  bsoncxx::document::value document = bsoncxx::from_json(request.body());
  bsoncxx::document::view view = document.view();
  MineModel temp;
  temp.getFromBsonDocument(view);
  bsoncxx::document::value insertDocument = temp.convertToBsonDocument();
  std::string resString = (DatabaseHandler::insertDocument("minesTest", insertDocument) == true ? ("Rudnik uspešno vstavljen!") : ("Pri vstavlajnju rudnika je prišlo do napake!"));
  bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
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
  auto sv = mineIdElement.get_string().value;
  std::string str_val(sv.data(), sv.size());
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
  auto sv = mineIdElement.get_string().value;
  std::string str_val(sv.data(), sv.size());
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
  auto sv = mineIdElement.get_string().value;
  std::string str_val(sv.data(), sv.size());
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