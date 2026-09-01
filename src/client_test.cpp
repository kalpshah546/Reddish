#include <iostream>
#include "../include/ReddishServer.h"
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){
    int client=socket(AF_INET,SOCK_STREAM,0);
    if(client<0){
        std::cerr<<"Failed to create socket"<<std::endl;
        return 1;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(1234);
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(client,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0){
        std::cerr<<"Failed to connect to server"<<std::endl;
        return 1;
    }
    std::cout<<"Connected to server"<<std::endl;
    while(true){
        std::string input;
        std::cout<<"> ";
        std::getline(std::cin,input);
        if(input=="exit") break;
        send(client,input.c_str(),input.size(),0);
        char buffer[1024];
        ssize_t bytesRead=recv(client,buffer,sizeof(buffer)-1,0);
        if(bytesRead<0){
            std::cerr<<"Failed to receive response from server"<<std::endl;
            break;
        }
        buffer[bytesRead]='\0';
        std::cout<<buffer;
    }
    close(client);
    return 0;
}