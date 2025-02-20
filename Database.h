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
        std::unordered_map<std::string,Table> tables;

    public:
        Database(){}
        Database(const std::string& dbName) : name(dbName) {}
        Database(const Database& other): name(other.name), tables(other.tables) {}
        
        std::string create_table(std::string tableName, std::vector <std::string> cols);
        void setName(const std::string& dbname);
        
        bool tableExists(const std::string& tableName) const;

        Table& getTable(const std::string& tableName);

        void addTable(const Table& table)
        {
            tables[table.getName()] = table;
        }

        std::string getName(){return name;}

        std::unordered_map<std::string,Table>& getAllTables(){return tables;}

        void create_table_from_load(std::string nume_tabel,std::string column_name,std::string type);

        bool hasTable(std::string tableName);

        void add_column(std::string tablename,std::string columnname, std::string type);

        void delete_column(std::string tablename,std::string columnanme);
        
        ~Database()
        {
            name="";
            tables.clear();
        }
};

//database