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
};

#endif