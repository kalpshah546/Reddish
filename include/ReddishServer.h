#ifndef REDDISH_SERVER_H
#define REDDISH_SERVER_H


#include <atomic>
#include <string>
class ReddishServer{
    private:
        int port;
        int server_socket;
        std::atomic<bool> running;
    public:
        ReddishServer(int port);
        void run();
        void shutdown();
};

#endif