#include <iostream>

#include "Model/UserModel.hpp"

void testUserValidation()
{
    timeStamp now = std::chrono::duration_cast<timeStamp>(
        std::chrono::system_clock::now().time_since_epoch());
    UserModel user1("ime", "test@gmail.com", "Geslo12asd!", now, now, now);
    std::cout << "TEST: 1 pričakovano [1] dobljeno:[" << user1.validateUserData(REGISTRATION) << "]\n";

    UserModel user2("ime", "testgmail.com", "Geslo12asd!", now, now, now);
    std::cout << "TEST: 2 pričakovano [0] dobljeno:[" << user2.validateUserData(REGISTRATION) << "]\n";

    UserModel user3("ime", "test@gmail.com", "Gesloasd!", now, now, now);
    std::cout << "TEST: 3 pričakovano [0] dobljeno:[" << user3.validateUserData(REGISTRATION) << "]\n";

    UserModel user4("ime", "testgmail.com", "Geslo12asd", now, now, now);
    std::cout << "TEST: 4 pričakovano [0] dobljeno:[" << user4.validateUserData(REGISTRATION) << "]\n";

    UserModel user5("ime", "testgmail.com", "asdeslo12asd!", now, now, now);
    std::cout << "TEST: 5 pričakovano [0] dobljeno:[" << user5.validateUserData(REGISTRATION) << "]\n";

    UserModel user6("ime", "", "Geslo12asd!", timeStamp::zero(), timeStamp::zero(), timeStamp::zero());
    std::cout << "TEST: 6 pričakovano [1] dobljeno:[" << user6.validateUserData(LOGIN) << "]\n";

    UserModel user7("ime", "", "Geslo12asd!", timeStamp::zero(), timeStamp::zero(), timeStamp::zero());
    std::cout << "TEST: 7 pričakovano [1] dobljeno:[" << user7.validateUserData(UPDATE) << "]\n";

    UserModel user8("ime", "", "Geslo12asd!", timeStamp::zero(), timeStamp::zero(), timeStamp::zero());
    std::cout << "TEST: 8 pričakovano [0] dobljeno:[" << user8.validateUserData(UPDATE_EMAIL) << "]\n";

    UserModel user9("ime", "testmail@gmail.com", "Geslo12asd!", timeStamp::zero(), timeStamp::zero(), timeStamp::zero());
    std::cout << "TEST: 9 pričakovano [1] dobljeno:[" << user9.validateUserData(UPDATE_EMAIL) << "]\n";
}