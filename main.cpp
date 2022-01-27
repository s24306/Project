#include <iostream>
#include <cstdlib>
#include <string>
#include "CustomerAccount.h"
#include "DBLite.h"
#include <algorithm>
#include "TickCount.h"


void printLogo();
void printMessage();
void mainMenu(DBLite *sqldb);
bool showMainMenuOptions(DBLite *sqldb);
bool isCardNumberValid(const std::string& cardNo);
void accountCreation(DBLite *sqldb);
std::string luhnAlgorithm(DBLite *sqldb);
void accountLogin(DBLite *sqldb);
bool loggedAccountOptions(DBLite *sqldb, Account account);
void addIncomeToTheAccount(DBLite *sqldb, Account account);
void transferMoney(DBLite *sqldb, Account account);
void closeCurrentAccount(DBLite *sqldb, Account account);
bool cardNumberExists(const std::vector<string>& list, const std::string& string);
bool isNumber(const string& str);

int main() {
    DBLite sqldb;
    sqldb.createTable();
    printLogo();
    printMessage();
    mainMenu(&sqldb);
    std::cout << "See you soon!" << std::endl;
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
    while(true){
        std::cout << "1. Create an account\n"
                  << "2. Log into account\n"
                  << "0. Exit\n";
        unsigned int chosenOption;
        std::cin >> chosenOption;
        if(cin.fail()){
            std::cout << "Not only you cannot read, but also you cannot differentiate between numbers and letters???\n";
            cin.clear();
            cin.ignore(512, '\n');
        }
        switch (chosenOption) {
            case 1:
                accountCreation(sqldb);
                break;
            case 2:
                accountLogin(sqldb);
                break;
            case 0:
                return true;
            default:
                std::cout << "That's not a viable command, did you consider learning how to read???\n";
        }
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

std::string luhnAlgorithm(DBLite *sqldb){
    std::vector<string> accountsNumbersVector = sqldb->getAllAccountsNumbers();
    while (true){
        std::string randomAccountNumber = "4";
        srand(GetTickCount());
        for (int i = 0; i < 15; i++){
            int randomNum = rand() % 10;
            randomAccountNumber.append(std::to_string(randomNum));
        }
        if ((isCardNumberValid(randomAccountNumber))
            && !cardNumberExists(accountsNumbersVector, randomAccountNumber)){
            return randomAccountNumber;
        }
    }
}

bool cardNumberExists(const std::vector<string>& list, const std::string& string){
    return std::count(list.begin(),list.end(),string);
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
    account.accountNum = luhnAlgorithm(sqldb);
    account.accountPIN = generatePIN();
    std::cout << "Your card has been created" << std::endl
              << "Your account number: " << account.accountNum << std::endl
              << "Your PIN: " << account.accountPIN << std::endl << std::endl;
    sqldb->insertData(&account.accountNum[0], &account.accountPIN[0]);
}

void accountLogin(DBLite *sqldb){
    while(true) {
        std::string userInputLogin;
        std::string userInputPIN;
        std::cout << "Enter your card number: " << std::endl;
        std::cin >> userInputLogin;
        std::cout << "Enter your PIN: " << std::endl;
        std::cin >> userInputPIN;
        if (sqldb->checkCredentials(&userInputLogin[0], &userInputPIN[0])) {
            std::vector<std::string> accountData = sqldb->getAccountDetails(&userInputLogin[0]);
            Account account;
            account.accountNum = accountData[0];
            account.accountBalance = std::stol(accountData[1]);
            std::cout << "You have successfully logged in!\n\n";
            loggedAccountOptions(sqldb, account);
            break;
        } else {
            std::cout << "Uh oh, wrong card number or PIN!\n" << std::endl;
            break;
        }
    }
}

bool loggedAccountOptions(DBLite *sqldb, Account account){
    while(true) {
        std::vector<string> newBalance = sqldb->getAccountDetails(&account.accountNum[0]);
        account.accountBalance = account.accountBalance = std::stol(newBalance[1]);
        std::cout << "1. Balance\n"
                  << "2. Add income\n"
                  << "3. Do transfer\n"
                  << "4. Close account\n"
                  << "0. Log out\n";
        unsigned int chosenOption;
        std::cin >> chosenOption;
        if(cin.fail()){
            std::cout << "Not only you cannot read, but also you cannot differentiate between numbers and letters???\n";
            cin.clear();
            cin.ignore(512, '\n');
        }
        switch (chosenOption) {
            case 1:
                std::cout << "Your balance is: " << (unsigned long long int)account.accountBalance << endl << endl;
                break;
            case 2:
                addIncomeToTheAccount(sqldb, account);
                std::cout << "Balance added successfully\n";
                break;
            case 3:
                transferMoney(sqldb, account);
                break;
            case 4:
                closeCurrentAccount(sqldb, account);
                return false;
            case 0:
                return false;
            default:
                std::cout << "That's not a viable command, did you consider learning how to read???\n";
        }
    }
}

void addIncomeToTheAccount(DBLite *sqldb, Account account){
    while(true){
        unsigned long long int deposit;
        std::cout << "Insert amount to deposit: ";
        std::cin >> deposit;
        if(deposit < 0){
            std::cout << "amount cannot be less than 0" << std::endl;
        } else {
            account.accountBalance += deposit;
            sqldb->updateAccountBalance(&account.accountNum[0],
                        &std::to_string(account.accountBalance)[0]);
            break;
        }
    }
}

void transferMoney(DBLite *sqldb, Account account){
    while(true){
        long moneyTransfer;
        Account targetAccount;
        std::string targetNumber;
        std::cout << "Insert card number to which the money is to be transferred: ";
        std::cin >> targetNumber;
        if (!isNumber(targetNumber)){
            std::cout << "This isn't really a number, is it?\n";
        }
        targetAccount.accountNum = targetNumber;
        std::vector<std::string> accountData = sqldb->getAccountDetails(&targetAccount.accountNum[0]);
        targetAccount.accountBalance = std::stol(accountData[1]);
        std::cout << "Insert amount to transfer: ";
        std::cin >> moneyTransfer;
        if(cin.fail()){
            std::cout << "Unfortunately you cannot transfer letters. And yes, we also would love to send the D.\n";
            cin.clear();
            cin.ignore(512, '\n');
        }
        if(moneyTransfer < 0){
            std::cout << "Amount cannot be less than 0" << std::endl;
        } else if (moneyTransfer > account.accountBalance){
            std::cout << "You don't have enough money! Try again.\n";
        } else {
            account.accountBalance -= moneyTransfer;
            sqldb->updateAccountBalance(&account.accountNum[0],
                                        &std::to_string(account.accountBalance)[0]);
            targetAccount.accountBalance += moneyTransfer;
            sqldb->updateAccountBalance(&targetAccount.accountNum[0],
                                        &std::to_string(targetAccount.accountBalance)[0]);
            break;
        }
    }
}

bool isNumber(const string& str){
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

void closeCurrentAccount(DBLite *sqldb, Account account){
    sqldb->deleteAccount(&account.accountNum[0]);
}

void printLogo(){
    std::cout << "////////////////////////////////////////////////////////////////" << std::endl
              << "_______       ________         __      ___________     __    __" << std::endl
              << "|   ___ \\   |_______  |       /  \\    |____  _____|   |  | /  /" << std::endl
              << "|  |   | |          | |      / /\\ \\       |  |        |  |/  /" << std::endl
              << "|  |___| |          | |     / /  \\ \\      |  |        |     /" << std::endl
              << "|   ____/           | |    / /____\\ \\     |  |        |    |" << std::endl
              << "|  |         _      | |   /  ______  \\    |  |        |     \\" << std::endl
              << "|  |        | |_____| |  / /        \\ \\   |  |        |  |\\  \\" << std::endl
              << "|  |        |________/  / /          \\ \\  |  |        |  | \\  \\" << std::endl
              << "////////////////////////////////////////////////////////////////" << std::endl << std::endl;
}

void printMessage() {
    std::cout << "Message to all of our dear customers: \n"
                 "Due to the recent inflation, the polish government decided to drop the zloty "
                 "subunit called \"grosz\"."
                 "\nFrom now on our bank is operating only on Polish Zloty without any decimal places.\n"
                 "All of the balances on the accounts will be rounded down to the nearest integer.\n"
                 "Sorry for the inconvenience\n\n";
}
