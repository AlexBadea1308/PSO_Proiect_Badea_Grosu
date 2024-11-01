#include "Database.h"

void Database::create_table(std::string tableName, std::vector<std::string> cols)
{
    Table* new_table = new Table(tableName);

    for(auto& i : cols)
    {
        size_t start = i.find('(');
        size_t comma = i.find(','); 
        size_t end = i.find(')');   

        std::string colname,type;

        if (start != std::string::npos && comma != std::string::npos && end != std::string::npos) {
             colname = i.substr(start + 1, comma - start - 1);
             type = i.substr(comma + 1, end - comma - 1);
        }   
        else
        {
            std::cerr<<"error\n";
        }

        new_table->createColumn(colname,type);
        
    }

    tables[tableName] = *new_table;

    return;
}

void Database::setName(const std::string& dbname)
{
    name = dbname;
}
