#include "Database.h"

void Database::print_db()
{
    for (const auto& row : table) {
        for (const auto& col : row) {
            std::cout << col << "\t"; 
        }
        std::cout << std::endl;
    }
}
