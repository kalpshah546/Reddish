#include <iostream>
#include "../include/ReddishServer.h"
#include <thread>
#include <chrono>
using namespace std;

int main(int argc,char*argv[]){
    int port=6379;
    if(argc>=2) port=stoi(argv[1]);
    ReddishServer server(1234);
    
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