#include "Server.h"

void Server::handleHello(int clientSocket)
{   
    std::string helloString = "Hello Client!\n";
    send(clientSocket, helloString.c_str(),helloString.size(), 0);
}

std::string readFileToString(const std::string& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filePath);
    }

    // Read the entire file into a string
    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return fileContents;
}

std::string Server::handleLogin(std::string user,std::string pass)
{
    std::ifstream file("users.txt");
    
    if (!file) {
        return "Error: Could not open users.txt";
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        std::istringstream iss(line);
        std::string user_name, password;
        
        iss >> user_name >> password;
        
        if (user_name == user && password == pass) {
            return "Login successful";
        }
    }

    return "Invalid username or password";
}

std::string Server::handleAllow(std::vector<std::string> userstoAllow)
{
    if (userstoAllow.size() < 3) 
    { 
        return "Error: Invalid command format. Usage: allow <dbname> <user1> <user2> ...";
    }

    std::string dbName = userstoAllow[1];
    std::unordered_set<std::string> newUsers(userstoAllow.begin() + 2, userstoAllow.end());//skip allow and dbname

    std::ifstream permsFile("perm.txt");
    if (!permsFile.is_open()) {
        return "Error: Could not open perm.txt file.";
    }

    std::string line;
    bool dbFound = false;
    std::vector<std::string> fileContents;
    
    while (std::getline(permsFile, line)) {
        std::istringstream iss(line);
        std::string db;
        iss >> db;

        if (db == dbName) {
            dbFound = true;

            // Store the current users in a set to avoid duplicates
            std::unordered_set<std::string> existingUsers;
            std::string user;
            while (iss >> user) {
                existingUsers.insert(user);
            }

            for (const auto& newUser : newUsers) {
                existingUsers.insert(newUser);
            }

            std::ostringstream updatedLine;
            updatedLine << dbName;
            for (const auto& user : existingUsers) {
                updatedLine << " " << user;
            }
            fileContents.push_back(updatedLine.str());
        } else {
            // keep lines of other databases unchanged
            fileContents.push_back(line);
        }
    }

    permsFile.close();

    if (!dbFound) {
        return "Database not found.\n";
    }

    std::ofstream outFile("perm.txt");
    if (!outFile.is_open()) {
        return "Error: Could not open perm.txt for writing.";
    }

    for (const auto& updatedLine : fileContents) {
        outFile << updatedLine << "\n";
    }

    outFile.close();
    return "Permissions updated successfully.";
}

std::string Server::handleDeny(std::vector<std::string> userstoDeny)
{   
    if (userstoDeny.size() < 3) {
        return "Error: Insufficient arguments. Expected format: deny <db_name> <user1> <user2> ...";
    }

    std::string dbName = userstoDeny[1];
    std::unordered_set<std::string> usersToDeny(userstoDeny.begin() + 2, userstoDeny.end());

    std::ifstream inputFile("perm.txt");
    if (!inputFile.is_open()) {
        return "Error: Could not open perm.txt";
    }

    std::vector<std::string> lines;
    std::string line;
    bool dbFound = false;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string currentDbName;
        iss >> currentDbName;

        if (currentDbName == dbName) {
            dbFound = true;

            std::ostringstream updatedLine;
            updatedLine << dbName;
            std::string user;

            while (iss >> user) {
                if (usersToDeny.find(user) == usersToDeny.end()) {
                    updatedLine << " " << user;
                }
            }

            lines.push_back(updatedLine.str());
        } else {
            lines.push_back(line);
        }
    }

    inputFile.close();

    if (!dbFound) {
        return "Error: Database not found in perm.txt";
    }

    std::ofstream outputFile("perm.txt");
    if (!outputFile.is_open()) {
        return "Error: Could not write to perm.txt";
    }

    for (const auto& updatedLine : lines) {
        outputFile << updatedLine << "\n";
    }

    outputFile.close();
    return "Users denied access successfully";
}

bool Server::checkPermission(std::string dbName, std::string user)
{
    std::ifstream permsFile("perm.txt"); 
    if (!permsFile.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(permsFile, line)) 
    {
        std::istringstream iss(line);
        std::string db, userName;

        iss >> db;

        if (db == dbName) 
        {  
            while (iss >> userName) 
            {
                if (userName == user) 
                {
                    permsFile.close();
                    return true; 
                }
            }
        }
    }
    permsFile.close();
    return false;
}

std::string Server::insertRow(const std::string& tableName, const std::unordered_map<std::string, std::string>& values) 
{
    std::string response;
    if (!db->tableExists(tableName)) 
    {   response="Table "+tableName+" does not exist.\n";
        return response;
    }

    Table& table = db->getTable(tableName);

    for (const auto& [columnName, value] : values) 
    {
        if (!table.hasColumn(columnName)) 
        {
            response="Column "+columnName+" does not exist in table "+tableName+".\n";
            return response;
        }
    }
    if(table.insertRow(values)==false)
        response="Error data type in "+tableName+"!.\n";
    else
        response="Row inserted successfully into table "+tableName+".\n";
    return response;
}

std::string Server::handleInsert(std::string tableName, std::vector<std::string> com_vector)
{
        std::unordered_map<std::string, std::string> values;

        for (size_t i = 2; i < com_vector.size(); ++i) {
            size_t delimPos = com_vector[i].find(':');
            if (delimPos != std::string::npos) {
                std::string columnName = com_vector[i].substr(0, delimPos);
                std::string value = com_vector[i].substr(delimPos + 1);
                values.insert({columnName,value});
            }
        }
        return insertRow(tableName, values);
}



std::vector<std::string> Server::parseComm(std::string com)
{   
    std::vector<std::string> words;
    std::istringstream stream(com);
    std::string word;

    while (stream >> word) {
        words.push_back(word);
    }

    return words;
}

Database* Server::createDatabase(std::string dbName)
{       
        Database* new_db = new Database(dbName);
        return new_db;
}

std::string Server::handlePrintTable(const std::string &tableName)
{   std::string tab;
    if(db->hasTable(tableName)==1)
    {   tab=db->getTable(tableName).printTable();
        return tab;
    }
    tab="Table "+tableName+" does not exist!\n";
    return tab;
}

std::string Server::handleSave(std::string db_name)
{
    if (db_name!=db->getName())
    {   
        
        return std::string("You selected the wrong database to save!\n");
    }

    std::string filename = db_name + ".db";
    std::ofstream outFile(filename);

    if (!outFile.is_open())
    {   
        return std::string("Error saving database!");
    }

    for (const auto& table : db->getAllTables())
    {
        outFile <<"*\n"
        <<table.first<< "\n";
        Table t=table.second;
        for (auto& column : t.getAllColumns())
        {   
            Column c=column.second;
            outFile << column.first<<" "<<c.getType()<<" ";

            for (auto& value : column.second.getRows())
            {
                outFile << " " << value;
            }
            outFile << "\n";
        }
    }
    outFile << "*\n";
    outFile.close();
    return std::string("Database saved successfully!");
}

std::string Server::handleLoadDB(std::string db_name) 
{
    std::string filename = db_name + ".db";
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        return std::string("Error loading database!");
    }

    db=createDatabase(db_name);
    std::string line;
  
    while(std::getline(inFile, line)) 
    {  
        if(line!="*")
        {    
            std::istringstream iss(line);
            std::string name_table, type,name_column;
            iss >>name_table;

            std::getline(inFile,line);
            while(line!="*")
            {
                std::istringstream iss_2(line);

                iss_2>>name_column;
                iss_2>>type;
            
            if(!db->tableExists(name_table))
                db->create_table_from_load(name_table,name_column,type);
            else
                db->getTable(name_table).createColumn(name_column,type);
                
            std::string value;

            while (iss_2 >> value) 
            {  
               db->getTable(name_table).insertRowFromLoad(name_column,value);
            }
            
            std::getline(inFile, line);

            }
        }
    }

    inFile.close();
    return "Database loaded successfully!";
}

std::string Server::handleUpdate(std::string tableName, std::vector<std::string> com_vector)
{
    std::string response;
    if (!db->tableExists(tableName)) 
    {   response="Table "+tableName+" does not exist.\n";
        return response;
    }

    Table& table = db->getTable(tableName);
    
    std::string colSet=com_vector[3];
    std::string valueSet=com_vector[5];
    std::string colCond=com_vector[7];
    std::string op=com_vector[8];
    std::string valueCond=com_vector[9];

    if (!table.hasColumn(colSet)) 
    {
        response="Column "+com_vector[3]+" does not exist in table "+tableName+".\n";
        return response;
    }

    if (!table.hasColumn(colCond)) 
    {
        response="Column "+com_vector[3]+" does not exist in table "+tableName+".\n";
        return response;
    }
    
    return table.updateRow(colSet,valueSet,colCond,op,valueCond);
}


std::string Server::handleDelete(std::string tableName, std::vector<std::string> com_vector)
{
    std::string response;
    if (!db->tableExists(tableName)) 
    {   response="Table "+tableName+" does not exist.\n";
        return response;
    }

    Table& table = db->getTable(tableName);
    
    std::string colCond=com_vector[4];
    std::string op=com_vector[5];
    std::string valueCond=com_vector[6];

    if (!table.hasColumn(colCond)) 
    {
        response="Column "+com_vector[4]+" does not exist in table "+tableName+".\n";
        return response;
    }

    return table.deleteRow(colCond,op,valueCond);
}

std::string Server::handleAddColumn(std::string tablename,std::vector<std::string>com_vector)
{
    if(db->hasTable(tablename)==false)
    {
        return std::string("Table does not exist!\n");
    }

    if(db->getAllTables()[tablename].hasColumn(com_vector[2])==true)
    {
        return std::string("Column already exist!\n");
    }
    std::string columnname=com_vector[2];
    std::string type=com_vector[3];
    if(type=="INT"|| type=="int" || type=="DATE" || type=="date"|| type.find("nvarchar(") == 0 && type.back() == ')'||type.find("NVARCHAR(") == 0 && type.back() == ')')
        {
           db->add_column(tablename,columnname,type);
            return std::string("Column add succsessfully!\n");
        }
        else
        {
            return std::string("Invalid data type in column!\n");
        }
}

std::string Server::handleDeleteColumn(std::string tablename,std::vector<std::string> com_vector)
{
    if(db->hasTable(tablename)==false)
    {
        return std::string("Table does not exist!\n");
    }

    if(db->getAllTables()[tablename].hasColumn(com_vector[2])==false)
    {   
        return std::string("Column does not exist!\n");
    }
    std::string columnname=com_vector[2];

    db->delete_column(tablename,columnname);
    return std::string("Column delete successfully!\n");
}
bool Server::Initialize(int port) {

    int addrlen = sizeof(address);
    //char buffer[1024] = {0};

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        close(serverSocket);
        return -1;
    }

    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port); 

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port " << port << "...\n";

    return true;
}

ClientConnection Server::AcceptConnections() {
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    int addrlen=sizeof(address);

    int clientSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (clientSocket < 0) {
        std::cerr << "Accept failed\n";
        //exit(1);
    }

    std::cout << "Client connected\n";

    ClientConnection client_Connection(clientSocket);

    return client_Connection;

}

Server::~Server() {
    close(serverSocket);
    //free(db);
}

std::string getUser(int clientSocket)
{
    std::string req = "ASKUSER";
    send(clientSocket, req.c_str(),req.size(), 0);

    char buffer[1024] = { 0 };
    
    int bytesReceived = read(clientSocket, buffer, sizeof(buffer));

    if (bytesReceived == 0) {
        std::cout << "Client disconnected\n";
        close(clientSocket);
        return nullptr;
    }

    std::string response(buffer);

    return response;

}

int hasPermission=0;
std::string allowedDb = "";

void Server::handleReq(int clientSocket) {
    char buffer[1024] = { 0 };
    
    int bytesReceived = read(clientSocket, buffer, sizeof(buffer));

    if (bytesReceived == 0) {
        std::cout << "Client disconnected\n";
        close(clientSocket);
        return;
    }

    std::string request(buffer);
    std::vector <std::string> com_vector = parseComm(request);
    int ok=0;

       if(com_vector[0]=="create_database")
       {    
            std::string response;
            std::string filename = com_vector[1] + ".db";
            std::ifstream inFile(filename);
            if (inFile)
          {   
              response="Database already exists!";
          }
          else
          {
            db = createDatabase(com_vector[1]);
            std::cout<<"DB created!\n";
            response = "DB " + com_vector[1] + " created successfully!\n";
          }
          send(clientSocket, response.c_str(),response.size(), 0);
        ok=1;
       }

       if(com_vector[0]=="create_table"&& com_vector[1].find("[")!=0)
       {    
            std::string response=db->create_table(com_vector[1],com_vector);
            send(clientSocket,response.c_str(),response.size(), 0);
            ok=1;
       }

        if(com_vector[0]=="insert"&& db->hasTable(com_vector[1]))
       {    
            std::string response=handleInsert(com_vector[1],com_vector);
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
       }

       if(com_vector[0]=="print_table")
       {    
            std::string response = handlePrintTable(com_vector[1]);
            std::cout<<response<<std::endl;
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
       }

         if(com_vector[0]=="save")
       {    std::string response="";
            response=handleSave(com_vector[1]);
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
       }
          if(com_vector[0]=="load")
       {    std::string response;
            std::string username = getUser(clientSocket);

            if(checkPermission(com_vector[1],username)==1)
            {
               response = handleLoadDB(com_vector[1]);
               allowedDb = com_vector[1];
            }
            else{
                response="Permission to access "+com_vector[1]+" not granted. Try again!\n";
                //hasPermission = 0;
            }
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
       }

       if(com_vector[0]=="login")
        {   
            std::string response = handleLogin(com_vector[1],com_vector[2]);
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
        }

         if(com_vector[0]=="update"&& com_vector[2]=="set"&& com_vector[6]=="where")
        {   
            std::string response=handleUpdate(com_vector[1],com_vector);
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
        }

         if(com_vector[0]=="delete" && com_vector[1]=="from"&& com_vector[3]=="where")
        {   
            std::string response=handleDelete(com_vector[2],com_vector);
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
        }

        if(com_vector[0] == "allow")
        {   
            std::string response;

            if(allowedDb == com_vector[1])
            {
                response = handleAllow(com_vector);
            }
            else
            {
                response = "Permission not granted to allow users. Try to load DB first.\n";
            }
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
        }

        if(com_vector[0] == "deny")
        {   
            std::string response;

            if(allowedDb == com_vector[1])
            {
                response = handleDeny(com_vector);
            }
            else
            {
                response = "Permission not granted to deny users. Try to load DB first.\n";
            }
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
        }

        if(com_vector[0] == "?")
        {
            std::string response = readFileToString("help.txt");

            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
        }

         if(com_vector[0]=="add_column")
        {   
            std::string response=handleAddColumn(com_vector[1],com_vector);
            send(clientSocket,response.c_str(),response.size(),0);
            ok=1;
        }

          if(com_vector[0]=="delete_column")
        {   
            std::string response=handleDeleteColumn(com_vector[1],com_vector);
            send(clientSocket,response.c_str(),response.size(),0);
            ok=1;
        }

        else if(ok==0)
        {
            std::string response = "Wrong input!";
            send(clientSocket, response.c_str(),response.size(), 0);
        }
    return;
}

bool isSocketOpen(int socket) {
    char buffer;
    int result = recv(socket, &buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT);

    if (result == 0) {
        
        std::cerr << "Client disconnected.\n";
        return false;
    } else if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            
            return true;
        } else {
            
            std::cerr << "Socket error.\n";
            return false;
        }
    }
  
    return true;
}

void Server::handleClient(const ClientConnection& client) {
    bool flag = true;

    while (flag) {
        this->handleReq(client.getClientSocket());

        if (!isSocketOpen(client.getClientSocket())) {
            flag = false;
        }
    }
}

