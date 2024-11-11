#include "Server.h"

void Server::handleHello(int clientSocket)
{   
    std::string helloString = "Hello Client!\n";
    send(clientSocket, helloString.c_str(),helloString.size(), 0);
}

std::string handleLogin(std::string user,std::string pass)
{
    std::ifstream file("users.txt");
    
    if (!file) {
        return "Error: Could not open users.txt";
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        std::istringstream iss(line);
        std::string username, password;
        
        iss >> username >> password;
        
        if (username == user && password == pass) {
            return "Login successful";
        }
    }

    return "Invalid username or password";
}

std::string Server::insertRow(const std::string& tableName, const std::unordered_map<std::string, std::string>& values) 
{
    std::string response;
    if (!db->tableExists(tableName)) 
    {   response="Table "+tableName+" does not exists.\n";
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
{
    std::string tab = db->getTable(tableName).printTable();
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
        for (const auto& column : t.getAllColumns())
        {   
            Column c=column.second;
            outFile << column.first<<" "<<c.getType()<<" ";

            for (const auto& value : column.second.getRows())
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



bool Server::Initialize(int port) {

    int addrlen = sizeof(address);
    char buffer[1024] = {0};

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

}

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
            db = createDatabase(com_vector[1]);
            std::cout<<"DB created!\n";
            std::string response = "DB " + com_vector[1] + " created successfully!\n";
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
       }

       if(com_vector[0]=="create_table")
       {    
            db->create_table(com_vector[1],com_vector);
            std::cout<<"Table created successfully!\n";
            std::string response = "Table created successfully!\n";
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
       }

        if(com_vector[0]=="insert")
       {    
            std::string response=handleInsert(com_vector[1],com_vector);
            if(response.find("Row inserted successfully into table ") != std::string::npos)
            {
                handleSave(db->getName());
            }
            else
            {
                handleLoadDB(db->getName());
            }
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
       {    std::string response=handleLoadDB(com_vector[1]);
            send(clientSocket, response.c_str(),response.size(), 0);
            ok=1;
       }

       if(com_vector[0]=="login")
        {   
            std::string response=handleLogin(com_vector[1],com_vector[2]);
            send(clientSocket, response.c_str(),response.size(), 0);
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

