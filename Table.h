#pragma once
#include <iostream>
#include "Column.h"
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <map>
#include<cstdlib>

class Table{

    private:
        std::string name;
        std::map <std::string, Column> columns;

    public:
        Table() {}
        Table(const std::string& tableName) : name(tableName) {}

        void createColumn(std::string colName,std::string type);
        std::string printTable();

        bool insertRow(const std::unordered_map<std::string, std::string>& values);

        const std::string& getName() const {
            return name;
        }

    bool hasColumn(const std::string& columnName) const 
    {
        return columns.find(columnName) != columns.end();
    }

    std::map <std::string, Column> getAllColumns(){return columns;}

    void insertRowFromLoad(std::string column_name,std::string value);

    std::string updateRow(std::string colSet,std::string valueSet,std::string colCond,std::string op,std::string valueCond);
    bool evaluateIntCondition( std::string& colCond,  std::string& op,  std::string& colComp);
    bool evaluateNvarcharCondition( std::string& colCond,  std::string& op,  std::string& colComp);
    bool evaluateDateCondition( std::string& colCond,  std::string& op,  std::string& colComp);

    std::string deleteRow(std::string colCond,std::string op,std::string valueCond);

    ~Table()
    {
        name="";
        columns.clear();
    }
};