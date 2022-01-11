#ifndef BANKINGSYSTEM_DBLITE_H
#define BANKINGSYSTEM_DBLITE_H

#include <iostream>
#include <cstring>
#include <sqlite3.h>
#include <vector>

using namespace std;

class DBLite {

private:
    // Pointer to SQLite connection
    sqlite3 *db;

    // Save any error messages
    char *zErrMsg;

    // Save the result of opening the file
    int rc;

    // Saved SQL
    char* sql;

    // Compiled SQLite Statement
    sqlite3_stmt *stmt;

    // Create a callback function
    static int callback(void *NotUsed, int argc, char **argv, char **azColName) {

        // int argc: holds the number of results
        // (array) azColName: holds each column returned
        // (array) argv: holds each value

        for(int i = 0; i < argc; i++) {

            // Show column name, value, and newline
            cout << azColName[i] << ": " << argv[i] << endl;

        }

        // Insert a newline
        cout << endl;

        // Return successful
        return 0;
    }

    void checkDBErrors() {

        if( rc ){
            // Show an error message
            cout << "DB Error: " << sqlite3_errmsg(db) << endl;

            closeDB();

        }

    }


public:

    DBLite() {
        // Save the result of opening the file
        rc = sqlite3_open("Bank.db", &db);

        checkDBErrors();

        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stderr, "Opened database successfully\n");
        }


    }

    void createTable() {

        // Save SQL to create a table
        sql = "CREATE TABLE IF NOT EXISTS ACCOUNTS("  \
              "ID             INTEGER PRIMARY KEY     AUTOINCREMENT," \
              "NUMBER         TEXT    NOT NULL UNIQUE," \
              "PIN            TEXT     NOT NULL," \
              "BALANCE        INTEGER DEFAULT 0);";

        // Run the SQL
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Records created successfully\n");
        }

    }

    void insertData(char* cardNum, char* PIN) {

        char *query = NULL;

        // Build a string using asprintf()
        asprintf(&query, "INSERT INTO ACCOUNTS ('NUMBER', 'PIN') VALUES  ('%s', '%s');", cardNum, PIN);

        // Prepare the query
        //sqlite3_prepare(db, query, strlen(query), &stmt, NULL);
        rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Records created successfully\n");
        }

        // Test it
        //rc = sqlite3_step(stmt);

        // Finialize the usage
        //sqlite3_finalize(stmt);

        // Free up the query space
        free(query);

    }

    void showTable() {

        // Save SQL insert data
        sql = "SELECT * FROM ACCOUNTS;";

        // Run the SQL
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

        if( rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Operation done successfully\n");
        }

    }

    bool checkCredentials(char* cardNum, char* PIN) {

        char *query = NULL;

        // Build a string using asprintf()
        asprintf(&query, "SELECT NUMBER, PIN "
                         "FROM ACCOUNTS "
                         "WHERE NUMBER AND PIN IN ('%s', '%s');", cardNum, PIN);

        // Prepare the query
        //sqlite3_prepare(db, query, strlen(query), &stmt, NULL);
        rc = sqlite3_exec(db, query, NULL, 0, &zErrMsg);

        if( rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return 0;
        } else {
            fprintf(stdout, "You have successfully logged in!\n");
            return 1;
        }

    }

    void updateAccountBalance(char* cardNum, char* Balance) {

        char *query = NULL;

        // Build a string using asprintf()
        asprintf(&query, "UPDATE ACCOUNTS set Balance = '%s' where NUMBER='%s'", Balance, cardNum);

        // Prepare the query
        //sqlite3_prepare(db, query, strlen(query), &stmt, NULL);
        rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Balance added successfully\n");
        }

        // Test it
        //rc = sqlite3_step(stmt);

        // Finialize the usage
        //sqlite3_finalize(stmt);

        // Free up the query space
        free(query);
    }

    vector<string> getAccountDetails(char* cardNum) {

        char *query = NULL;
        vector<string> valuesVector{};

        // Build a string using asprintf()
        asprintf(&query, "SELECT NUMBER, BALANCE "
                         "FROM ACCOUNTS "
                         "WHERE NUMBER IN ('%s');", cardNum);

        // Prepare the query
        int rc = sqlite3_prepare_v2(db, query, strlen(query), &stmt, nullptr);
        while (sqlite3_step(stmt) != SQLITE_DONE) {
            string accNum = (const char *) (sqlite3_column_text(stmt, 0));
            //string accPIN = (const char *) (sqlite3_column_text(stmt, 1));
            int accBalance = sqlite3_column_int(stmt, 1);
            valuesVector.push_back(accNum);
            //valuesVector.push_back(accPIN);
            valuesVector.push_back(to_string(accBalance));
        }
        sqlite3_finalize(stmt);
        return valuesVector;

    }

    vector<string> getAllAccountsNumbers() {

        char *query = NULL;
        vector<string> cardsVector{};

        // Build a string using asprintf()
        asprintf(&query, "SELECT * FROM ACCOUNTS");

        // Prepare the query
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

        // Build a string using asprintf()
        asprintf(&query, "DELETE FROM ACCOUNTS WHERE NUMBER = '%s';", Number);

        // Prepare the query
        sqlite3_prepare(db, query, strlen(query), &stmt, NULL);

        // Test it
        rc = sqlite3_step(stmt);

        // Finialize the usage
        sqlite3_finalize(stmt);

        cout << "Your account has been deleted";
        free(query);

    }

    void closeDB() {

        // Close the SQL connection
        sqlite3_close(db);

    }



};

#endif //BANKINGSYSTEM_DBLITE_H
