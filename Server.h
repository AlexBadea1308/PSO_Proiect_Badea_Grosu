#pragma once
#include "ClientConnection.h"
#include "Database.h"
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include<functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <memory>
#include <thread>
#include <vector>
#include <sstream>


class Server {
private:
    int serverSocket;
    struct sockaddr_in address;
    std::string last_event;
    Database* db;
    std::string username;

public:

    Server() { db=new Database();}

    ~Server();

    bool Initialize(int port);

    ClientConnection AcceptConnections();

    void handleReq(int clientSocket);
    
    void handleClient(const ClientConnection& client);

    void handleHello(int clientSocket);

    std::string insertRow(const std::string &tableName, const std::unordered_map<std::string, std::string> &values);

    std::string handleInsert(std::string tableName, std::vector <std::string> com_vector);

    std::vector <std::string> parseComm(std::string com);

    Database* createDatabase(std::string dbName);

    std::string handlePrintTable(const std::string& tableName);

    std::string handleSave(std::string db_name);

    std::string handleLoadDB(std::string db_name);
    
    std::string handleUpdate(std::string tableName, std::vector<std::string> com_vector);

    std::string handleDelete(std::string tableName, std::vector<std::string> com_vector);

    bool checkPermission(std::string dbName, std::string user);

};