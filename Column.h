#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

class Column{

    private: 
        std::string name;
        std::string type;
        std::vector <std::string> rows;

    public:
        Column(){}
        Column(std::string colName,std::string type):name(colName),type(type){}

        std::string getName();
        std::vector<std::string> getRows();
        void setRow(int index,std::string rowValue);
        void addRow(const std::string& row);
        void deleteRow(int index);
        std::string getType(){return type;}
        ~Column()
        {
            name="";
            type="";
            rows.clear();
        }
        bool verifyType(std::string value);
        bool validateInt(const std::string& value);
        bool validateNvarchar(const std::string& value, int maxLength);
        bool validateDate(const std::string& value);
        int getSizeRows();
};