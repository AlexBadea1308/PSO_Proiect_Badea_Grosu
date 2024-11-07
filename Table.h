#pragma once
#include <iostream>
#include "Column.h"
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <map>

class Table{

    private:
        std::string name;
        std::map <std::string, Column> columns;

    public:
        Table() {}
        Table(const std::string& tableName) : name(tableName) {}

        void createColumn(std::string colName,std::string type);
        std::string printTable()const;

        bool insertRow(const std::unordered_map<std::string, std::string>& values);

        const std::string& getName() const {
            return name;
        }

    bool hasColumn(const std::string& columnName) const 
    {
        return columns.find(columnName) != columns.end();
    }

    std::map <std::string, Column>& getAllColumns(){return columns;}
};

/* create Database

create_table tablename (colname,type) (colname,type)
add tablename colname type


*/