#include "Table.h"

void Table::createColumn(std::string colName, std::string type)
{
    Column* new_column = new Column(colName,type);

    columns[colName] = *new_column;

    return;
}