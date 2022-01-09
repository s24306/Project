#ifndef BANKINGSYSTEM_CUSTOMERACCOUNT_H
#define BANKINGSYSTEM_CUSTOMERACCOUNT_H

#include <string>

struct Account{
    std::string accountNum;
    double accountBalance {0};
    std::string accountPIN;

    std::string checkBalance();
    bool IsPasswordMatching();

};

#endif //BANKINGSYSTEM_CUSTOMERACCOUNT_H
