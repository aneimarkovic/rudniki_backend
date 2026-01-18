// Created by Anei Markovič 26.4.2025
#include <bsoncxx/builder/stream/document.hpp>
#include <iostream>
#include <sodium.h> 

#include "HttpServer.hpp"
#include "DatabaseHandler.hpp"

#include "RouterUtil/Router.hpp"
#include "Controller/MineController.hpp"
#include "Controller/UserController.hpp"
#include "Controller/HelmetDetectionController.h"
#include "WebSocket/WebsocketController.hpp"
#include "Controller/NotificationController.h"

void getAndPrintAllScrapperData()
{
    std::cout << "Pridobivam podatke od scrapperja" << std::endl;
    bsoncxx::document::view filters{};
    std::string collName = "mines";
    std::vector<bsoncxx::document::value> scrapperVec = DatabaseHandler::fetchMultipleDocuments(collName, filters);
    std::string temp = "";
    for (auto &&i : scrapperVec)
    {
        // std::cout << bsoncxx::to_json(i) << std::endl;
        temp += bsoncxx::to_json(i);
    }

    std::cout << temp;
}

int main()
{
    try
    {
        HttpServer::createOAuthJWT();
        // std::cout << "ACCESS TOKEN: " << HttpServer::accessToken << std::endl;

        if (sodium_init() < 0) {
            throw std::runtime_error("Failed to initialize libsodium!");
        }

//         testDatabaseInsert();
//         getAndPrintAllScrapperData();
        DatabaseHandler::initialize();

//        Mine routes
        Router::createPostRoute("/mine/save", MineController::saveMine);
        Router::createPostRoute("/mine/addInfrastructure", MineController::addInfrastructure);
        Router::createPostRoute("/mine/addMineral", MineController::addMineral);
        Router::createPostRoute("/mine/addWorker", MineController::addWorker);
        Router::createPostRoute("/mine/generateMinerals", MineController::generateMineralsValue);
        Router::createPostRoute("/mine/filters", MineController::getFilteredMines);
        Router::createPostRoute("/mine/getMinesByYear", MineController::getMinesByYear);
        Router::createPostRoute("/mine/addHistory/:id", MineController::addMineHistory);
        Router::createPostRoute("/mine/createPasswords", MineController::createPasswords);

        Router::createGetRoute("/mine/get/:id", MineController::getMine);
        Router::createGetRoute("/", MineController::getScrapperMines);
        Router::createGetRoute("/mine/", MineController::getAllMines);
        Router::createGetRoute("/:id", MineController::searchBar);
        Router::createGetRoute("/mine/statistics", MineController::getStatistics);
        Router::createGetRoute("/mine/history/:id", MineController::getMineHistory);
        Router::createGetRoute("/mine/getPasswords", MineController::getPasswords);

        Router::createDeleteRoute("/mine/delete/:id", MineController::deleteMine);
        Router::createDeleteRoute("/mine/deleteWorker", MineController::deleteWorker);
        Router::createDeleteRoute("/mine/deleteInfrastructure", MineController::deleteInfrastructure);
        Router::createDeleteRoute("/mine/deleteMineral", MineController::deleteMineral);

        Router::createPutRoute("/mine/update", MineController::updateMine);

//        User routes
        Router::createPostRoute("/user/", UserController::loginUser);
        Router::createPostRoute("/user/save", UserController::saveUser);

        Router::createPutRoute("/user/update", UserController::updateUser);

        Router::createGetRoute("/user/mines/:id", MineController::getMineBasedOnOwner);
        Router::createGetRoute("/user/:id", UserController::getUser);

        // WEBHOOKS
        WebSocketController::createWebSocket("/hooks/rudnikSubscribe", "rudnikSubscribe");

        // BLOCKCHAIN
        Router::createGetRoute("/blockchain/mine/:data", MineController::callBlockchainService);
        Router::createGetRoute("/blockchain/stop", MineController::callBlockchainService);

        // CV Algorithm
        Router::createPostRoute("/helmets/save", HelmetDetectionController::getCVAlgorithmData);

        // NOTIFICATIONS
        Router::createPostRoute("/notification/register", NotificationController::receiveDeviceInfoFromApp);
        Router::createPostRoute("/notification/send", NotificationController::receiveMessageInfoFromApp);

        //DatabaseHandler::create2dsphereIndex("bordersTest", "geometry") ? std::cout << "OK\n" : std::cout << "NE OK\n";

        HttpServer server("127.0.0.1", "8080");
        server.runServer();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "ERROR: " << ex.what() << "\n";
    }
}
