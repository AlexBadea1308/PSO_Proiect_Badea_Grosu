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
};