#include "Column.h"

std::string Column::getName()
{
    return name;
}


const std::vector<std::string> &Column::getRows() const
{
    return rows;
}

void Column::addRow(const std::string &row)
{
    rows.push_back(row);
}
