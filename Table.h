#pragma once
#include <iostream>
#include "Column.h"
#include <string>
#include <vector>
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
};

/* create Database

create_table tablename (colname,type) (colname,type)
add tablename colname type


*/