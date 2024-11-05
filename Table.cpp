#include "Table.h"

void Table::createColumn(std::string colName, std::string type)
{
    Column* new_column = new Column(colName,type);

    columns[colName] = *new_column;

    return;
}

void Table::printTable()  {
        if (columns.empty()) {
            std::cout << "Table " << name << " is empty." << std::endl;
            return;
        }

        size_t numRows = 0;
        if (!columns.empty()) {
            numRows = columns.begin()->second.getRows().size();
        }

        // Print column headers
        for (const auto& col : columns) {
            std::cout << std::setw(15) << col.first;
        }
        std::cout << std::endl;

        // Print rows of data
        for (size_t row = 0; row < numRows; ++row) {
            for (const auto& col : columns) {
                const auto& colData = col.second.getRows();
                if (row < colData.size()) {
                    std::cout << std::setw(15) << colData[row];
                } else {
                    std::cout << std::setw(15) << ""; // Blank space if column has fewer rows
                }
            }
            std::cout << std::endl;
        }
    }
