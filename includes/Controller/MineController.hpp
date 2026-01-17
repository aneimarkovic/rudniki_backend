/*
    Created by Anei Markovic 22.5.2025
    Razred za delo z rudniki, dodajanje, posodabljanje, brisanje...
*/
#ifndef MINECONTROLLER_H
#define MINECONTROLLER_H

#include "RouterUtil/Router.hpp"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include  <bsoncxx/builder/stream/array.hpp>

#include "Model/UserModel.hpp"

class MineController
{
public:
    static void saveMine(const request &request, response &response, Router* r);
    static void getMine(const request &request, response &response, Router* r);
    static void deleteMine(const request &request, response &response, Router* r);
    static void addInfrastructure(const request &request, response &response, Router* r);
    static void addMineral(const request &request, response &response, Router* r);
    static void addWorker(const request &request, response &response, Router* r);
    static void generateMineralsValue(const request &request, response &response, Router* r);
    static void getScrapperMines(const request &request, response &response, Router* r);
    static void getFilteredMines(const request &request, response &response, Router* r);
    static void getMineBasedOnOwner(const request &request, response &response, Router *r);
    static void getAllMines(const request &request, response &response, Router *r);
    static void searchBar(const request &request, response &response, Router* r);
    static void getMinesByYear(const request &request, response &response, Router* r);
    static void deleteWorker(const request &request, response &response, Router* r);
    static void deleteInfrastructure(const request &request, response &response, Router* r);
    static void deleteMineral(const request &request, response &response, Router* r);
    static void updateMine(const request &request, response &response, Router* r);
    static void getStatistics(const request& request, response& response, Router* r);

    static void getMineHistory(const request& request, response& response, Router* r);
    static void addMineHistory(const request& request, response& response, Router* r);

    static void callBlockchainService(const request& request, response& response, Router* r);
    static std::string blockchainClient(std::string path);

    static void saveToBlockchain(std::string rawData);
};

#endif