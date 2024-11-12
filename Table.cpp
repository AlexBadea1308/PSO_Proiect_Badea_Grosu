#include "Table.h"

void Table::createColumn(std::string colName, std::string type)
{
    Column* new_column = new Column(colName,type);

    columns[colName] = *new_column;

    return;
}

std::string Table::printTable() const {
    if (columns.empty()) {
        return "Table " + name + " is empty.\n";
    }

    std::ostringstream oss;

    size_t numRows = 0;
    if (!columns.empty()) {
        numRows = columns.begin()->second.getRows().size();
    }

    for (const auto& col : columns) {
        oss << std::setw(15) << col.first; 
    }
    oss << '\n';

    for (size_t row = 0; row < numRows; ++row) {
        for (const auto& col : columns) 
        {
            const auto& colData = col.second.getRows();

            if (row < colData.size()) {
                oss << std::setw(15) << colData[row];
            } else {
                oss << std::setw(15) << ""; // spatiu ca sa arate mai frumos
            }
        }
        oss << '\n';
    }

    return oss.str();
}

bool Table::insertRow(const std::unordered_map<std::string, std::string> &values)
{   
    int ok=1;
     for (auto& [columnName, column] : columns) 
     {
        auto it = values.find(columnName);
        if (it != values.end())
        {   
            if(column.verifyType(it->second)==false)
             {ok=0;
             break;
             }
        }
    }

    if(ok==1)
    {
        for (auto& [columnName, column] : columns) 
     {
        auto it = values.find(columnName);
        if (it != values.end())
        {   
           column.addRow(it->second);
        }
    }
    return true;
    }
    return false;
}

void Table::insertRowFromLoad(std::string column_name, std::string value)
{   
    for(auto& it: columns)
    {
        if(it.first==column_name)
        {
            it.second.addRow(value);
        }
    }
}

std::string Table::updateRow(std::string colSet, std::string valueSet, std::string colCond, std::string op, std::string valueCond)
{
    int index=0;

    for(auto &it : columns[colCond].getRows())
    {   
    }
}
