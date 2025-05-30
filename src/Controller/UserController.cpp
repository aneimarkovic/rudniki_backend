// Created by Anei Markovič 10.5.2025
#include "Controller/UserController.hpp"
#include "DatabaseHandler.hpp"

/*
	Funckija pokliče metode DatabaseHandler in shrani uporabnika
*/
void UserController::saveUser(const request& request, response& response, Router* router)
{
	
}

//Funkcija, ki prijavi uporabnika in vrne njegov id
void UserController::loginUser(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    UserModel temp;
    temp.getFromBsonDocumentLogin(view);

//    std::cout << temp.toString() << std::endl;

    temp.hashPassword();
    std::optional<bsoncxx::oid> id = temp.authUserData();
    std::string status = "Napaka ob prijavi!";
    if(id){
        status = id->to_string();
    }
    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << status << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view viewTemp = documentTemp.view();
    std::string jsonStr = bsoncxx::to_json(viewTemp);

    response.body() = jsonStr;
}

void UserController::getUser(const request &request, response &response, Router* r){
    auto filters = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{r->UrlArguments[0]}));

    std::optional<bsoncxx::document::value> userDoc = DatabaseHandler::fetchSingleDocument("users", filters);
    if (userDoc)
    {
        bsoncxx::document::view viewTemp = userDoc->view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);
        response.body() = jsonStr;
    }
    else
    {
        std::cout << "Neobstaja\n";
    }
}