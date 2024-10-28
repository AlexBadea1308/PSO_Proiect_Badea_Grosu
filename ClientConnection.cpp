#include "ClientConnection.h"

int ClientConnection::Receive(char* buffer, int bufferSize) {
    return read(clientSocket, buffer, bufferSize);
}

int ClientConnection::Send(const char* data, int dataSize) {
    return send(clientSocket, data, dataSize, 0);
}

ClientConnection::~ClientConnection() {
    //close(clientSocket);
}

int ClientConnection::getClientSocket()const
{
    return clientSocket;
}