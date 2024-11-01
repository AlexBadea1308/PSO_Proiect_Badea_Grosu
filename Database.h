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
        Database(const std::string& dbName) : name(dbName) {}
        //Database(const Database& other): name(other.name), tables(other.tables) {}
        
        void create_table(std::string tableName, std::vector <std::string> cols);
        void setName(const std::string& dbname);
        

};