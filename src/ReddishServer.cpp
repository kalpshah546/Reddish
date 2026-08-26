#include "ReddishServer.h"
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;
static ReddishServer*global_server=nullptr;

ReddishServer::ReddishServer(int port):port(port),server_socket(-1),running(true){
    global_server=this;
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

}