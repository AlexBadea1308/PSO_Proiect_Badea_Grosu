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

    int Receive(char* buffer, int bufferSize);

    int Send(const char* data, int dataSize);

    ~ClientConnection();

    int getClientSocket()const;
};