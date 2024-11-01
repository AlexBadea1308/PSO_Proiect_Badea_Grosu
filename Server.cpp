#include "Server.h"

void Server::handleHello(int clientSocket)
{   
    std::string helloString = "Hello Client!\n";
    send(clientSocket, helloString.c_str(),helloString.size(), 0);
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
        exit(1);
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

    if (request == "HELLO") {
        std::cout << "Hello requested\n";
        handleHello(clientSocket);
        return;
    }
    else
    {
       std::vector <std::string> com_vector = parseComm(request);
       //std::cout<<com_vector[0]<<" "<<com_vector[1]<<std::endl;

       if(com_vector[0]=="create_database")
       {    
            db = createDatabase(com_vector[1]);
            std::cout<<"DB created!\n";
       }

       if(com_vector[0]=="create_table")
       {
            db->create_table(com_vector[1],com_vector);
            std::cout<<"Table created successfully!\n";
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

