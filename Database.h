#include <iostream>
#include <string>
#include <vector>

class Database {

    private:
        int cols,rows;
        std::vector <std::vector <std::string>> table;

    public:
        Database(int cols, int rows):cols(cols),rows(rows){}
        Database(){}
        int getCols(){return cols;};
        int getRows(){return rows;};
        void print_db();
        

};