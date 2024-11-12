#include "Table.h"

void Table::createColumn(std::string colName, std::string type)
{
    Column* new_column = new Column(colName,type);

    columns[colName] = *new_column;

    return;
}

std::string Table::printTable()
{
    if (columns.empty()) {
        return "Table " + name + " is empty.\n";
    }

    std::ostringstream oss;

    size_t numRows = 0;
    if (!columns.empty()) {
        numRows =columns.begin()->second.getRows().size();
    }

    for (const auto& col : columns) {
        oss << std::setw(15) << col.first; 
    }
    oss << '\n';

    for (size_t row = 0; row < numRows; ++row) {
        for (auto& col : columns) 
        {
            auto colData = col.second.getRows();

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

bool Table::evaluateIntCondition(std::string &colCond,std::string &op,std::string &colComp)
{
    int condValue = std::stoi(colCond);
    int compValue = std::stoi(colComp);
   
    if (op == "=") {
        return condValue == compValue;
    } else if (op == "!=") {
        return condValue != compValue;
    } else if (op == ">") {
        return condValue < compValue;
    } else if (op == "<") {
        return condValue > compValue;
    } else if (op == ">=") {
        return condValue <= compValue;
    } else if (op == "<=")
        return condValue >= compValue;
    return false;
}

bool Table::evaluateNvarcharCondition( std::string &colCond,  std::string &op,std::string &colComp)
{
    if (op == "=") {
        return colCond == colComp;
    } else if (op == "!=") {
        return colCond != colComp;
    } else if (op == ">") {
        return colCond < colComp;
    } else if (op == "<") {
        return colCond > colComp;
    } else if (op == ">=") {
        return colCond <= colComp;
    } else if (op == "<=") {
        return colCond >= colComp;
    } 
        return false;
}

bool Table::evaluateDateCondition( std::string &colCond,  std::string &op,  std::string &colComp)
{
     int dayCond, monthCond, yearCond;
    int dayComp, monthComp, yearComp;
    char dash;  // pentru a ignora caracterele '-'

    std::stringstream ssCond(colCond);
    ssCond >> dayCond >> dash >> monthCond >> dash >> yearCond;

    std::stringstream ssComp(colComp);
    ssComp >> dayComp >> dash >> monthComp >> dash >> yearComp;

    if (op == "=") 
    {
        if (yearCond == yearComp) {
            if (monthCond == monthComp) {
                return dayCond == dayComp;
            }
            return false;
        }
        return false;
    } else if (op == "!=") {
        if (yearCond != yearComp) return true;
        if (monthCond != monthComp) return true;
        return dayCond != dayComp;
    } else if (op == ">") {
        if (yearCond > yearComp) return true;
        if (yearCond < yearComp) return false;
        if (monthCond > monthComp) return true;
        if (monthCond < monthComp) return false;
        return dayCond > dayComp;
    } else if (op == "<") {
        if (yearCond < yearComp) return true;
        if (yearCond > yearComp) return false;
        if (monthCond < monthComp) return true;
        if (monthCond > monthComp) return false;
        return dayCond < dayComp;
    } else if (op == ">=") {
        if (yearCond > yearComp) return true;
        if (yearCond < yearComp) return false;
        if (monthCond > monthComp) return true;
        if (monthCond < monthComp) return false;
        return dayCond >= dayComp;
    } else if (op == "<=") {
        if (yearCond < yearComp) return true;
        if (yearCond > yearComp) return false;
        if (monthCond < monthComp) return true;
        if (monthCond > monthComp) return false;
        return dayCond <= dayComp;
    } 
        return false;
}

std::string Table::updateRow(std::string colSet, std::string valueSet, std::string colCond, std::string op, std::string valueCond)
{
    int index=0,ok_cond=0,ok_set=0,count=0;

    //ok_cond verificam daca tipul de date de la conditie este acelasi cu cel din coloana conditie

    if((columns[colCond].getType()=="INT"||columns[colCond].getType()=="int")&&columns[colCond].validateInt(valueCond))
    {
        ok_cond=1;
    }
    else if(columns[colCond].getType().rfind("NVARCHAR(",0)==0||columns[colCond].getType().rfind("nvarchar(",0)==0)
    {
        size_t start = columns[colCond].getType().find('(') + 1;
        size_t end = columns[colCond].getType().find(')');
        int maxLength = std::stoi(columns[colCond].getType().substr(start, end - start));
        if (columns[colCond].validateNvarchar(valueCond,maxLength))
        {
            ok_cond=2;
        }
    }
    else if(columns[colCond].getType()=="DATE"||(columns[colCond].getType()=="date")&&columns[colCond].validateDate(valueCond))
    {
        ok_cond=3;
    }


    // verificam daca tipul de date de la set este acelasi cu cel din coloana de set

    if((columns[colSet].getType()=="INT"||columns[colSet].getType()=="int")&&columns[colSet].validateInt(valueSet))
    {
        ok_set=1;
    }
    else if(columns[colSet].getType().rfind("NVARCHAR(",0)==0||columns[colSet].getType().rfind("nvarchar(",0)==0)
    {
        size_t start = columns[colSet].getType().find('(') + 1;
        size_t end = columns[colSet].getType().find(')');
        int maxLength = std::stoi(columns[colSet].getType().substr(start, end - start));
        if (columns[colSet].validateNvarchar(valueSet,maxLength))
        {
            ok_set=1;
        }
    }
    else if(columns[colSet].getType()=="DATE"||(columns[colSet].getType()=="date")&&columns[colSet].validateDate(valueSet))
    {
        ok_set=1;
    }

    //updatam randurile care respecta conditia data de operator
    for(auto &it : columns[colCond].getRows())
    {   switch (ok_cond) {
    case 1: 
        if (ok_set==1&&evaluateIntCondition(valueCond,op,it)){
            columns[colSet].setRow(index,valueSet);
            count++;
        }
        break;
    case 2: 
        if (ok_set==1&&evaluateNvarcharCondition(valueCond, op, it)) {
            columns[colSet].setRow(index,valueSet);
            count++;
        }
        break;

    case 3:
        if (ok_set==1&&evaluateDateCondition(valueCond,op,it))
        {
            columns[colSet].setRow(index,valueSet);
            count++;
        }
        break;
    default:
        return std::string("INVALID TYPE OF DATE OR TYPE OF VALUE");
        break;
    }
    index++;
    } 
    std::string response="Succes! ";
    std::stringstream ss;
    ss <<count;
    response+=ss.str()+" rows affected!";
    return response;
}