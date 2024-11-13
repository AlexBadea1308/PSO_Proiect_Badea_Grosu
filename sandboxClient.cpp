#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345


int userLogin(int clientSocket,std::string user,std::string pass)
{   
    std::string request;

    request = "login " + user + " " + pass;

    send(clientSocket, request.c_str(), request.size(), 0);

    char buffer[10240] = { 0 };
    int bytesReceived = read(clientSocket, buffer, sizeof(buffer));

    if (bytesReceived == 0) {
            std::cerr << "Receive failed\n";
            close(clientSocket);            
            return 1;
            }

    std::string response(buffer);

    if (response == "Login successful")
        return 1;
    else
        return 0;

    return -1;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }
    else
        std::cout<<"Connected to server!\n";

    
    int checkLogin = 0;

    while (checkLogin != 1)
    {
        std::string user, pass;
        std::cout << "\nEnter username:\n";
        std::getline(std::cin, user);
        std::cout << "\nEnter password:\n";
        std::getline(std::cin, pass);
        checkLogin = userLogin(sock,user,pass);

        if (checkLogin != 1)
        std::cout << "\nInvalid credentials. Try again";
    }

    while (true) {
        
        std::cout << "\nEnter the request : ";
        std::string req;
        std::getline(std::cin, req);

        if (req == "exit")
        {   
            close(sock);
            return 1;
        }
            send(sock,req.c_str(),req.size(),0);
            char buffer[1024] = { 0 };
            int bytesReceived = read(sock, buffer, sizeof(buffer));

            if (bytesReceived == 0) {
            std::cerr << "Receive failed\n";
            close(sock);            
            return 1;
            }
            
            std::cout<<buffer;
            continue;
    }  
    return 0;
}
