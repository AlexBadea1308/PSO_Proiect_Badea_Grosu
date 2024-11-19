#include "Column.h"

std::string Column::getName()
{
    return name;
}


std::vector<std::string> Column::getRows()
{
    return rows;
}

void Column::setRow(int index,std::string rowValue)
{
    rows[index]=rowValue;
}

void Column::addRow(const std::string &row)
{
    rows.push_back(row);
}

void Column::deleteRow(int index)
{   
    rows.erase(rows.begin() + index);
}

bool Column::verifyType(std::string value)
{
    if ((type == "INT") ||(type=="int")) 
    {
        if (validateInt(value)==1)
            return true;
    } else if (type.rfind("NVARCHAR(", 0) == 0||type.rfind("nvarchar(", 0) == 0)
    { 
        size_t start = type.find('(') + 1;
        size_t end = type.find(')');
        int maxLength = std::stoi(type.substr(start, end - start));
        
        if (validateNvarchar(value, maxLength)) 
            return true;
        }
         else if (type == "DATE"||type=="date")
        if (validateDate(value))
           return true;

        return false;
}

bool Column::validateNvarchar(const std::string &value, int maxLength)
{    
    return value.size() <= maxLength;
}
bool Column::validateDate(const std::string &value)
{
    if (value.size() != 10 || value[2] != '-' || value[5] != '-') 
    {
        return false;
    }
    for (size_t i = 0; i < value.size(); i++) 
    {
        if (i != 2 && i != 5 && !std::isdigit(value[i])) 
        {
            return false;
        }
    }
    return true;
}
int Column::getSizeRows()
{
    return rows.size();
}
bool Column::validateInt(const std::string &value)
{   

    for (char ch : value)
        if (!std::isdigit(ch)) 
        {
            return false;
        }
    return true;
}
