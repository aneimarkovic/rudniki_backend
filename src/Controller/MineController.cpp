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
  bsoncxx::document::value doccumentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
  bsoncxx::document::view viewTemp = doccumentTemp.view();
  std::string jsonStr = bsoncxx::to_json(viewTemp);

  response.body() = jsonStr;
}

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

void MineController::deleteMine(const request &request, response &response, Router *r)
{
  auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{r->UrlArguments[0]}));

  std::string resString = (DatabaseHandler::deleteDocument("minesTest", filters) == true ? ("Rudnik uspešno izbrisan!") : ("Pri brisanju rudnika je prišlo do napake!"));
  bsoncxx::document::value doccumentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
  bsoncxx::document::view viewTemp = doccumentTemp.view();
  std::string jsonStr = bsoncxx::to_json(viewTemp);

  response.body() = jsonStr;
}