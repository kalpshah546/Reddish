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
#include <fcntl.h>
#include <sys/epoll.h>
#include <algorithm>
#include <cerrno>


using namespace std;
static ReddishServer*global_server=nullptr;


void ReddishServer::handleSignal(int signal){
    if(global_server){
        cout<<"Signal "<<signal<<" received. Shutting down server..."<<endl;
        global_server->shutdown();
    }
}
void ReddishServer::setupSignalHandler(int signal){
    std::signal(SIGINT,handleSignal);
}
static void setNonBlocking(int fd){
    int flags=fcntl(fd,F_GETFL,0);
    if(flags==-1){
        cerr<<"Error getting socket flags"<<endl;
        return;
    }
    if(fcntl(fd,F_SETFL,flags|O_NONBLOCK)==-1){
        cerr<<"Error setting socket to non-blocking"<<endl;
    }
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
void ReddishServer::acceptConnection(){
    int client_socket=accept(server_socket,nullptr,nullptr);
    if(client_socket<0){
        if(running){
            cerr<<"Error accepting connection"<<endl;
        }
        return;
    }
    setNonBlocking(client_socket);
    connections.push_back(Conn{client_socket,true,false,false,{},{}});
    epoll_event event{};
    event.events=EPOLLIN;
    event.data.fd=client_socket;
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_socket,&event)<0){
        cerr<<"Error adding client socket to epoll"<<endl;
        close(client_socket);
        connections.pop_back();
    }
}
void ReddishServer::handleConnection(int fd,uint32_t events){
    auto it=std::find_if(connections.begin(),connections.end(),[fd](const Conn&c){return c.fd==fd;});
    if(it==connections.end()){
        cerr<<"Connection not found for fd "<<fd<<endl;
        return;
    }
    Conn&conn=*it;
    if(events & EPOLLIN){
        char buffer[1024];
        ssize_t bytes=recv(fd,buffer,sizeof(buffer)-1,0);
        if(bytes<=0){
            conn.want_close=true;
            return;
        }
        std::string request(buffer,bytes);
        ReddishCommandHandler cmd;
        std::string response=cmd.processCommand(request);
        send(fd,response.c_str(),response.size(),0);
    }
    if(events & EPOLLOUT){
        if(!conn.outgoing.empty()){
            ssize_t bytes=send(fd,conn.outgoing.data(),conn.outgoing.size(),0);
            if(bytes>0){
                conn.outgoing.erase(conn.outgoing.begin(),conn.outgoing.begin()+bytes);
            }
        }
    }
    if(events & (EPOLLHUP | EPOLLERR)){
        conn.want_close=true;
    }
}
void ReddishServer::closeConnection(int fd){
    auto it=std::find_if(connections.begin(),connections.end(),[fd](const Conn&c){return c.fd==fd;});
    if(it!=connections.end()){
        epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,nullptr);
        close(fd);
        connections.erase(it);
    }
}
void ReddishServer::run(){
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket<0){
        cerr<<"Error creating socket"<<endl;
        return;
    }
    int opt=1;
    if(setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0){
        cerr<<"Error setting socket options"<<endl;
        return;
    }
    setNonBlocking(server_socket);
    sockaddr_in server_addr{};
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
    server_addr.sin_port=htons(port);
    if(bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
        cerr<<"Error binding socket"<<endl;
        close(server_socket);
        return;
    }
    if(listen(server_socket,10)<0){
        cerr<<"Error listening on socket"<<endl;
        close(server_socket);
        return;
    }
    epoll_fd=epoll_create1(0);
    if(epoll_fd<0){
        cerr<<"Error creating epoll instance"<<endl;
        close(server_socket);
        return;
    }
    epoll_event event{};
    event.events=EPOLLIN;
    event.data.fd=server_socket;
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,server_socket,&event)<0){
        cerr<<"Error adding server socket to epoll"<<endl;
        close(server_socket);
        close(epoll_fd);
        return;
    }
    cout<<"Server running on port "<<port<<endl;
    while(running){
        epoll_event events[10];
        int n=epoll_wait(epoll_fd,events,10,-1);
        for(int i=0;i<n;i++){
            if(events[i].data.fd==server_socket){
                acceptConnection();
            }
            else{
                handleConnection(events[i].data.fd,events[i].events);
            }
        }
        for(auto it=connections.begin();it!=connections.end();){
            if(it->want_close){
                closeConnection(it->fd);
                it=connections.erase(it);
            }
            else{
                ++it;
            }
        }
    }
    if(!ReddishDatabase::getInstance().dump("reddish_dump.rdb")){
        cerr<<"Error dumping database to file"<<endl;
    }
    else{
        cout<<"Database dumped to reddish_dump.rdb"<<endl;
    }
}