#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


class ClientConnection {
private:
    int clientSocket;

public:
    ClientConnection(const ClientConnection& other) : clientSocket(other.clientSocket) {}

    ClientConnection(int socket) : clientSocket(socket) {}

    ~ClientConnection();

    int getClientSocket()const;
};