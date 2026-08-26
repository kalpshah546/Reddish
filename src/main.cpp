#include <iostream>
#include "../include/ReddishServer.h"
#include <thread>
#include <chrono>
using namespace std;

int main(int argc,char*argv[]){
    int port=1234;
    if(argc>=2) port=stoi(argv[1]);
    ReddishServer server(port);
    
    //dump the data in disk every 5 minutes for persistence
    thread persistenceThread([](){
        while(true){
            this_thread::sleep_for(chrono::seconds(300));
            // dump the database
        }
    });
    persistenceThread.detach();
    server.run();

}