#pragma once
#include <iostream>
#include "Table.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

class Database {

    private:
        std::string name;
        std::unordered_map<std::string, Table> tables;

    public:
        Database(){}
        void print_db();
        std::string Database::createTable(const std::string& tableName);
        

};