#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "CustomerAccount.h"
#include <cmath>
#include <ctime>
#include <sqlite3.h>
#include "DBLite.h"


void printLogo();
void mainMenu(DBLite *sqldb);
bool showMainMenuOptions(DBLite *sqldb);
bool isCardNumberValid(const std::string& cardNo);
void accountCreation(DBLite *sqldb);
void accountLogin(DBLite *sqldb);

int main() {
    DBLite sqldb;
    sqldb.createTable();
    printLogo();
    mainMenu(&sqldb);
    std::cout << "bye lol" << std::endl;
    sqldb.closeDB();
    return 0;
}

void mainMenu(DBLite *sqldb){
    bool isSessionClosed = false;
    while (!isSessionClosed){
        isSessionClosed = showMainMenuOptions(sqldb);
    }
}

bool showMainMenuOptions(DBLite *sqldb){
    std::cout << "1. Create an account\n"
              << "2. Log into account\n"
              << "0. Exit\n";
    unsigned int chosenOption;
    std::cin >> chosenOption;
    switch (chosenOption){
        case 1:
            accountCreation(sqldb);
            break;
        case 2:
            accountLogin(sqldb);
            break;
        case 0:
            return true;
            break;
        default:
            return false;
    }
}

std::string generatePIN(){
    std::string randomPIN = "";
    srand(time(nullptr));
    for (int i = 0; i < 4; i++){
        int randomNum = rand() % 10;
        randomPIN.append(std::to_string(randomNum));
    }
    return randomPIN;
}

std::string luhnAlgorithm(){
    while (true){
        std::string randomAccountNumber = "4";
        srand(time(nullptr));
        for (int i = 0; i < 15; i++){
            int randomNum = rand() % 10;
            randomAccountNumber.append(std::to_string(randomNum));
        }
        if (isCardNumberValid(randomAccountNumber)){
            return randomAccountNumber;
        }
    }
}

bool isCardNumberValid(const std::string& cardNo){
    int nDigits = cardNo.length();

    int nSum = 0, isSecond = false;
    for (int i = nDigits - 1; i >= 0; i--) {

        int d = cardNo[i] - '0';

        if (isSecond == true)
            d = d * 2;

        nSum += d / 10;
        nSum += d % 10;

        isSecond = !isSecond;
    }
    return (nSum % 10 == 0);
}

void accountCreation(DBLite *sqldb){
    Account account;
    account.accountNum = luhnAlgorithm();
    account.accountPIN = generatePIN();
    std::cout << "Your card has been created" << std::endl
              << "Your account number: " << account.accountNum << std::endl
              << "Your PIN: " << account.accountPIN << std::endl << std::endl;
    sqldb->insertData(&account.accountNum[0], &account.accountPIN[0]);
    sqldb->showTable();
}

void accountLogin(DBLite *sqldb){
    std::string userInputLogin;
    std::string userInputPIN;
    std::cout << "Enter your card number: " << std::endl;
    std::cin >> userInputLogin;
    std::cout << "Enter your PIN: " << std::endl;
    std::cin >> userInputPIN;
    if (sqldb->checkCredentials(&userInputLogin[0], &userInputPIN[0])){
        std::cout << "jest git" << std::endl;
    } else {
        std::cout << "Uh oh, wrong card number or PIN!" << std::endl;
    }
}

/*cur.execute('SELECT number, pin FROM card WHERE number and pin in (?, ?)', (login, password))
found = cur.fetchone()
if found:
print("You have successfully logged in!")
print("Wrong card number or PIN!")
login_number = login
inside_account()
else:
print("Wrong card number or PIN!")*/

void printLogo(){
    std::cout << "//////////////////////////////////////" << std::endl
              << "_______   _________    ___    ____________ ___   ___" << std::endl
              << "|   ___ \\   _________    ___    ____________ ___   ___" << std::endl
              << "|  |   | |    _________    ___    ____________ ___   ___" << std::endl
              << "|  |___| |   _________    ___    ____________ ___   ___" << std::endl
              << "|   ____/   _________    ___    ____________ ___   ___" << std::endl
              << "|  |   _________    ___    ____________ ___   ___" << std::endl
              << "|  |        _________    ___    ____________ ___   ___" << std::endl
              << "|  |        _________    ___    ____________ ___   ___" << std::endl
              << "//////////////////////////////////////" << std::endl << std::endl;
}
