#include "ReddishServer.h"
#include "ReddishCommandHandler.h"
#include "ReddishDatabase.h"
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <csignal>

using namespace std;
static ReddishServer*global_server=nullptr;

void ReddishServer::handleSignal(int signal){
    if(global_server){
        cout<<"Received signal "<<signal<<", shutting down the server..."<<endl;
        global_server->shutdown();
    }
    exit(signal);
}
void ReddishServer::setupSignalHandler(int signal){
    std::signal(SIGINT,handleSignal);
}
ReddishServer::ReddishServer(int port):port(port),server_socket(-1),running(true){
    global_server=this;
    setupSignalHandler(SIGINT);
}

void ReddishServer::shutdown(){
    running =false;
    if(server_socket!=-1){
        close(server_socket);
    }
    cout<<"Server Shutdown complete"<<endl;
}

void ReddishServer::run(){
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket<0){
        cerr<<"Error creating a socket"<<endl;
    }
    int opt=1;
    setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=htonl(INADDR_ANY);

    if (bind(server_socket,(struct sockaddr*)&addr,sizeof(addr)) < 0) {
        cerr << "Error binding socket" << endl;
        close(server_socket);
        return;
    }

    if (listen(server_socket, SOMAXCONN) < 0) {
        cerr << "Error listening on socket" << endl;
        close(server_socket);
        return;
    }


    cout << "Reddish server listening on port "<< port << endl;
    vector<std::thread>threads;
    ReddishCommandHandler cmd;
    while(running){
        int client_socket=accept(server_socket,nullptr,nullptr);
        if(client_socket<0){
            if(running){
                cerr<<"Error accepting connection"<<endl;
            }
        }
        threads.emplace_back([client_socket,&cmd](){
            while(true){
                char buffer[1024];
                ssize_t bytes=recv(client_socket,buffer,sizeof(buffer)-1,0);
                if(bytes<=0){
                    break;
                }
                std::string request(buffer,bytes);
                std::string response=cmd.processCommand(request);
                send(client_socket,response.c_str(),response.size(),0);
            }
            close(client_socket);
        });
        for(auto& t:threads){
            if(t.joinable()){
                t.join();
            }
        }
        //before shutting down , persist the data to disk
        if(!ReddishDatabase::getInstance().dump("reddish.db")){
            cerr<<"Failed to dump the database"<<endl;
        }
        else{
            cout<<"Database dumped successfully"<<endl;
        }

    }

}