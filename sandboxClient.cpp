#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345

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

    while (true) {
        
        std::cout << "\nEnter the request : ";
        std::string req;
        std::getline(std::cin, req);

        if (req == "exit")
        {
            close(sock);
            return 1;
        }

        if (req == "hello")
        {   
            std::string helloReq="HELLO";
            send(sock,helloReq.c_str(),helloReq.size(),0);

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

        else
        {
            size_t pos = req.find(';');
    
            if (pos != std::string::npos) 
            {
                req.erase(pos);
            }

            send(sock,req.c_str(),req.size(),0);

        }
    
    }  
    return 0;
}
