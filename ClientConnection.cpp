#include "ClientConnection.h"


ClientConnection::~ClientConnection() {
    //close(clientSocket);
}

int ClientConnection::getClientSocket()const
{
    return clientSocket;
}