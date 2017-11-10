#pragma once
#include "sqlite3.h"
#include <stdio.h>
#include "MyGuid_Lib.h"


static int callbackGUID(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i<argc; i++) {
        std::string col = azColName[i];
        std::string value = argv[i];
        if (strcmp(azColName[i], "quote(id)") == 0) {
            xg::Guid g = dbToGuid(argv[i]);
            value = g.str();
            col = "ID";
        }
        printf("%s = %s\n", col.data(), value.data() ? value.data() : "NULL");
    }
    printf("\n");
    return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

sqlite3 *openDB(char* filename) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc = sqlite3_open(filename, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    }
    else {
        fprintf(stdout, "Opened database successfully\n");
    }

    return db;
}

void create(char *sql, sqlite3 *db) {
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Table created successfully\n");
    }
}

void insert(char *sql, sqlite3 *db) {
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Records created successfully\n");
    }
}

void select(char *sql, sqlite3 *db) {
    char *zErrMsg = 0;
    const char* data = "Callback function called";
    int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Operation done successfully\n");
    };
}

void closeDB(sqlite3 *db) {
    try {
        int rc = sqlite3_close(db);
        fprintf(stdout, "Closed database successfully\n");
    }
    catch (const char* msg) {
        fprintf(stderr, "SQL error: %s\n", msg);
    }
}