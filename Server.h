#pragma once
#include "ClientConnection.h"
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include<functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>


class Server {
private:
    int serverSocket;
    struct sockaddr_in address;
    std::string last_event;

public:

    Server() {};

    ~Server();

    bool Initialize(int port);

    ClientConnection AcceptConnections();

    void handleReq(int clientSocket);
    
    void handleClient(const ClientConnection& client);

    void handleHello(int clientSocket);

};