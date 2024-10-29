#include "Database.h"


std::string Database::createTable(const std::string& tableName) {
    if (database.find(tableName) != database.end()) {
        return "Table already exists.";
    }
    database[tableName] = {};
    return "Table created successfully.";
}
