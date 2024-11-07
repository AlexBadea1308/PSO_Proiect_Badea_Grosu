#include "Server.h"

void Server::handleHello(int clientSocket)
{   
    std::string helloString = "Hello Client!\n";
    send(clientSocket, helloString.c_str(),helloString.size(), 0);
}

void Server::insertRow(const std::string& tableName, const std::unordered_map<std::string, std::string>& values) {
    if (!db->tableExists(tableName)) {
        std::cerr << "Table " << tableName << " does not exist.\n";
        return;
    }

    Table& table = db->getTable(tableName);

    for (const auto& [columnName, value] : values) {
        if (!table.hasColumn(columnName)) {
            std::cerr << "Column " << columnName << " does not exist in table " << tableName << ".\n";
            return;
        }
    }

    table.insertRow(values);
    std::cout << "Row inserted successfully into table " << tableName << ".\n";
}

void Server::handleInsert(std::string tableName, std::vector<std::string> com_vector)
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
        insertRow(tableName, values);
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
    if (db_name.compare(db->getName()) != 0)
    {
        return "You selected the wrong database to save!\n";
    }

    std::string filename = db_name + ".db";
    std::ofstream outFile(filename);

    if (!outFile.is_open())
    {
        return "Error saving database!";
    }

    for (const auto& table : db->getAllTables())
    {
        outFile << table.first<< "\n";
        Table t=table.second;
        for (const auto& column : t.getAllColumns())
        {
            outFile << column.first<<" ";

            for (const auto& value : column.second.getRows())
            {
                outFile << " " << value;
            }

            outFile << "\n";
        }

        outFile << "*\n";
    }
    outFile.close();
    return "Database saved successfully!";
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
    char buffer[102400] = { 0 };
    
    int bytesReceived = read(clientSocket, buffer, sizeof(buffer));

    if (bytesReceived == 0) {
        std::cout << "Client disconnected\n";
        close(clientSocket);
        return;
    }

    std::string request(buffer);

    if (request == "HELLO") {
        std::cout << "Hello requested\n";
        handleHello(clientSocket);
        return;
    }
    else
    {
       std::vector <std::string> com_vector = parseComm(request);

       if(com_vector[0]=="create_database")
       {    
            db = createDatabase(com_vector[1]);
            std::cout<<"DB created!\n";
            std::string response = "DB " + com_vector[1] + " created successfully!\n";
            send(clientSocket, response.c_str(),response.size(), 0);
       }

       if(com_vector[0]=="create_table")
       {    
            db->create_table(com_vector[1],com_vector);
            std::cout<<"Table created successfully!\n";
            std::string response = "Table created successfully!\n";
            send(clientSocket, response.c_str(),response.size(), 0);
       }

        if(com_vector[0]=="insert")
       {    
            handleInsert(com_vector[1],com_vector);
            std::string response = "Row inserted successfully!\n";
            send(clientSocket, response.c_str(),response.size(), 0);
       }

       if(com_vector[0]=="print_table")
       {    
            std::string response = handlePrintTable(com_vector[1]);
            std::cout<<response<<std::endl;
            send(clientSocket, response.c_str(),response.size(), 0);
       }

         if(com_vector[0]=="save")
       {    std::string response;
            response=handleSave(com_vector[1]);
            //std::string response = "DB saved successfully\n";
            send(clientSocket, response.c_str(),response.size(), 0);
       }
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

