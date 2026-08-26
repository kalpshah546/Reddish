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
        std::cerr<<"Error creating socket"<<std::endl;
        return 1;
    }
    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(1234);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);
    if(connect(client,(struct sockaddr*)&addr,sizeof(addr))<0){
        std::cerr<<"Error connecting to server"<<std::endl;
        return 1;
    }
    std::cout<<"Connected to server"<<std::endl;
    std::string command="SET NAME Kalp";
    send(client,command.c_str(),command.size(),0);
    char buffer[1024];
    ssize_t bytes=recv(client,buffer,sizeof(buffer)-1,0);
    if(bytes<=0){
        std::cerr<<"Error receiving response"<<std::endl;
        return 1;
    }
    else{
        std::string response(buffer,bytes);
        std::cout<<"Response from server: "<<response<<std::endl;
    }
    close(client);
    return 0;
}