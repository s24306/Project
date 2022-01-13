cmake_minimum_required(VERSION 3.21)
project(BankingSystem)

set(CMAKE_CXX_STANDARD 11)

add_executable(BankingSystem main.cpp CustomerAccount.h DBLite.h TickCount.h)

target_link_libraries(BankingSystem PUBLIC sqlite3)
