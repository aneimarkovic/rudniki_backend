//Created by Anei Markovic 22.5.2025
#include <iostream>

#include "Controller/MineController.hpp"

//Funkcija shrani novi rudnik v bazo
 void MineController::saveMine(const request& request, response& response){
    //Json v MineModel
    std::cout << request.body() << "\n";
    std::cout << response.body() << "\n";
 }  