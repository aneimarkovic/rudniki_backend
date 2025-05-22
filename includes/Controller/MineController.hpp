/*
    Created by Anei Markovic 22.5.2025
    Razred za delo z rudniki, dodajanje, posodabljanje, brisanje...
*/
#ifndef MINECONTROLLER_H
#define MINECONTROLLER_H

#include "RouterUtil/Router.hpp"

class MineController
{
public:
    static void saveMine(const request &request, response &response);
};

#endif