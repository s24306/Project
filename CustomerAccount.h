#ifndef BANKINGSYSTEM_CUSTOMERACCOUNT_H
#define BANKINGSYSTEM_CUSTOMERACCOUNT_H

#include <string>

struct Account{
    std::string accountNum;
    unsigned long long int accountBalance {0};
    std::string accountPIN;
};

#endif //BANKINGSYSTEM_CUSTOMERACCOUNT_H
