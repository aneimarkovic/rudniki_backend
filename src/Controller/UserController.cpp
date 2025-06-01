// Created by Anei Markovič 10.5.2025
#include "Controller/UserController.hpp"
#include "DatabaseHandler.hpp"

/*
	Funckija pokliče metode DatabaseHandler in shrani uporabnika
*/
void UserController::saveUser(const request& request, response& response, Router* router)
{
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();


    UserModel user;
    user.getFromBsonDocument(view);
    user.hashPassword();
    bsoncxx::document::value temp = user.convertToBsonDocument();

    std::cout << user.toString() << std::endl;

    std::optional<bsoncxx::oid> id = DatabaseHandler::insertDocumentGetInsertId("users", temp);

    if(id){
        bsoncxx::oid actualId = *id;
        std::string jwt = HttpServer::createJWT(actualId);

        std::string cookie = "token=" + jwt + "; HttpOnly; Path=/; SameSite=Strict";
        response.set(http::field::set_cookie, cookie);

        std::cout << actualId.to_string() << std::endl;

        bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << actualId.to_string() << bsoncxx::builder::stream::finalize;
        bsoncxx::document::view viewTemp = documentTemp.view();
        std::string jsonStr = bsoncxx::to_json(viewTemp);
        response.body() = jsonStr;
    } else
    {
        response.body() = "Napaka pri dodajanju uporabnika!";
    }
}

//Funkcija, ki prijavi uporabnika in vrne njegov id
void UserController::loginUser(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    if (request.base().find("Cookie") != request.base().end()) {
        std::string cookieHeader = request.base()["Cookie"];
        std::cout << cookieHeader << std::endl;
    }


    UserModel temp;
    temp.getFromBsonDocumentLogin(view);

    temp.hashPassword();
    std::optional<bsoncxx::oid> id = temp.authUserData();
    std::string status = "Napaka ob prijavi!";
    if(id){
        status = id->to_string();
        bsoncxx::oid actualId = *id;
        std::string jwt = HttpServer::createJWT(actualId);

        std::string cookie = "token=" + jwt + "; HttpOnly; Path=/; SameSite=Strict";
        response.set(http::field::set_cookie, cookie);
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

void UserController::updateUser(const request &request, response &response, Router* r){
    bsoncxx::document::value document = bsoncxx::from_json(request.body());
    bsoncxx::document::view view = document.view();

    bsoncxx::builder::stream::document query;
    bsoncxx::document::element id = view["id"];
    auto stringView = id.get_string().value;
    std::string str_val(stringView.data(), stringView.size());
    bsoncxx::oid userID = bsoncxx::oid(str_val);
    query << "_id" << userID;

    bsoncxx::builder::stream::document updateQuery;
    bsoncxx::document::element element = view["username"];
    if(element && element.type() == bsoncxx::type::k_string){
        updateQuery << "username" << element.get_string().value;
    }

    element = view["email"];
    if(element && element.type() == bsoncxx::type::k_string){
        updateQuery << "email" << element.get_string().value;
    }
    bsoncxx::builder::stream::document updateSet;
    updateSet << "$set" << updateQuery.view();

    std::string resString = (DatabaseHandler::updateOneItem("users", query, updateSet) ? ("Uporabnik uspešno posodobljen!") : ("Pri posodabljanju uporabnika je prišlo do napake!"));
    bsoncxx::document::value documentTemp = bsoncxx::builder::stream::document{} << "message" << resString << bsoncxx::builder::stream::finalize;
    response.body() = bsoncxx::to_json(documentTemp.view());
}