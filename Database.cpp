#include "Database.h"
#include<sys/types.h>

std::string Database::create_table(std::string tableName, std::vector<std::string> cols)
{  
    if(tableExists(tableName))
    {
        return std::string("Table already exists!\n");
    }
    Table* new_table = new Table(tableName);
    int ok=1;
    for(auto& i : cols)
    {
        size_t start = i.find('[');
        size_t comma = i.find(','); 
        size_t end = i.find(']');   

        std::string colname,type;

        if (start != std::string::npos && comma != std::string::npos && end != std::string::npos) {
             colname = i.substr(start + 1, comma - start - 1);
             type = i.substr(comma + 1, end - comma - 1);
        }   
        else
        {
            continue;
            std::cerr<<"error\n";
        
        }
        if(type=="INT"|| type=="int" || type=="DATE" || type=="date"|| type.find("nvarchar(") == 0 && type.back() == ')'||type.find("NVARCHAR(") == 0 && type.back() == ')')
        {
            new_table->createColumn(colname,type);
        }
        else
        {
             ok=0;
            break;
        }
    }

    if(ok==1)
    {
        tables[tableName] = *new_table;
        return std::string("Table created succsessfully!\n");
    }
    return std::string("Problem at columns type! Check it!\n");
}

void Database::setName(const std::string& dbname)
{
    name = dbname;
}

bool Database::tableExists(const std::string &tableName) const
{
    return tables.find(tableName) != tables.end();
}

Table &Database::getTable(const std::string &tableName)
{
    auto it = tables.find(tableName);

    if (it == tables.end()) 
    {
        throw std::runtime_error("Table " + tableName + " does not exist.");
    }
    
    return it->second;
}

void Database::create_table_from_load(std::string nume_tabel, std::string column_name, std::string type)
{
    Table* new_table = new Table(nume_tabel);
    new_table->createColumn(column_name,type);
    tables[nume_tabel] = *new_table;
}

bool Database::hasTable(std::string tableName)
{
    return tables.find(tableName) != tables.end();
}

void Database::add_column(std::string tablename, std::string columnname, std::string type)
{
    tables[tablename].add_column(columnname,type);
}

void Database::delete_column(std::string tablename, std::string columnanme)
{
    tables[tablename].delete_column(columnanme);
}
