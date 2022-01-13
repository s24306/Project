#ifndef BANKINGSYSTEM_DBLITE_H
#define BANKINGSYSTEM_DBLITE_H

#include <iostream>
#include <cstring>
#include <sqlite3.h>
#include <vector>

using namespace std;

class DBLite {

private:
    sqlite3 *db;
    char *zErrMsg;
    int rc;
    char* sql;
    sqlite3_stmt *stmt;

    static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
        for(int i = 0; i < argc; i++) {
            cout << azColName[i] << ": " << argv[i] << endl;
        }
        cout << endl;
        return 0;
    }

    void checkDBErrors() {
        if( rc ){
            cout << "DB Error: " << sqlite3_errmsg(db) << endl;
            closeDB();
        }
    }


public:

    DBLite() {
        rc = sqlite3_open("Bank.db", &db);
        checkDBErrors();
        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stderr, NULL);
        }
    }

    void createTable() {

        sql = "CREATE TABLE IF NOT EXISTS ACCOUNTS("  \
              "ID             INTEGER PRIMARY KEY     AUTOINCREMENT," \
              "NUMBER         TEXT    NOT NULL UNIQUE," \
              "PIN            TEXT     NOT NULL," \
              "BALANCE        INTEGER DEFAULT 0);";

        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, NULL);
        }
    }

    void insertData(char* cardNum, char* PIN) {
        char *query = NULL;
        asprintf(&query, "INSERT INTO ACCOUNTS ('NUMBER', 'PIN') VALUES  ('%s', '%s');", cardNum, PIN);
        rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, NULL);
        }
        free(query);
    }

    void showTable() {
        sql = "SELECT * FROM ACCOUNTS;";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, NULL);
        }
    }

    bool checkCredentials(char* cardNum, char* PIN) {
        string dbPIN;
        char *query = NULL;
        asprintf(&query, "SELECT PIN "
            "FROM ACCOUNTS "
            "WHERE NUMBER IN ('%s');", cardNum);
        int rc = sqlite3_prepare_v2(db, query, strlen(query), &stmt, nullptr);
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            dbPIN = (const char *) (sqlite3_column_text(stmt, 0));
        }
        if( dbPIN ==  PIN) {
            return 1;
        } else {
            return 0;
        }
    }

    void updateAccountBalance(char* cardNum, char* Balance) {
        char *query = NULL;
        asprintf(&query, "UPDATE ACCOUNTS set Balance = '%s' where NUMBER='%s'", Balance, cardNum);
        rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, NULL);
        }
        free(query);
    }

    vector<string> getAccountDetails(char* cardNum) {
        char *query = NULL;
        vector<string> valuesVector{};
        asprintf(&query, "SELECT NUMBER, BALANCE "
                         "FROM ACCOUNTS "
                         "WHERE NUMBER IN ('%s');", cardNum);
        int rc = sqlite3_prepare_v2(db, query, strlen(query), &stmt, nullptr);
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            string accNum = (const char *) (sqlite3_column_text(stmt, 0));
            int accBalance = sqlite3_column_int(stmt, 1);
            valuesVector.push_back(accNum);
            valuesVector.push_back(to_string(accBalance));
        }
        sqlite3_finalize(stmt);
        return valuesVector;
    }

    vector<string> getAllAccountsNumbers() {
        char *query = NULL;
        vector<string> cardsVector{};
        asprintf(&query, "SELECT * FROM ACCOUNTS");
        int rc = sqlite3_prepare_v2(db, query, strlen(query), &stmt, nullptr);
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            string accNum = (const char *) (sqlite3_column_text(stmt, 1));
            cardsVector.push_back(accNum);
        }
        sqlite3_finalize(stmt);
        return cardsVector;
    }

    void deleteAccount(char* Number) {
        char *query = NULL;
        asprintf(&query, "DELETE FROM ACCOUNTS WHERE NUMBER IN ('%s');", Number);
        sqlite3_prepare(db, query, strlen(query), &stmt, NULL);
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        cout << "Your account has been deleted\n";
        free(query);
    }

    void closeDB() {
        // Close the SQL connection
        sqlite3_close(db);
    }
};

#endif //BANKINGSYSTEM_DBLITE_H
