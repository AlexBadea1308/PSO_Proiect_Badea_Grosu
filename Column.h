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
};