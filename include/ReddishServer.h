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

        //when user enters Ctrl+C, shutting down the server gracefully instead of abruptly killing the process
        static void handleSignal(int signal);
        void setupSignalHandler(int signal);
};

#endif