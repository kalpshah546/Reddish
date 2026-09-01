#ifndef REDDISH_SERVER_H
#define REDDISH_SERVER_H


#include <atomic>
#include <string>
#include <cstdint>
#include <vector>

class Conn{
    public:
    int fd=-1;
    bool want_read=false,want_write=false,want_close=false;
    std::vector<uint8_t>incoming;
    std::vector<uint8_t>outgoing;

    Conn(int fd,bool want_read,bool want_write,bool want_close,std::vector<uint8_t>incoming,std::vector<uint8_t>outgoing)
    :fd(fd),want_read(want_read),want_write(want_write),want_close(want_close),incoming(std::move(incoming)),outgoing(std::move(outgoing)){}
};
class ReddishServer{
    private:
        int port;
        int server_socket;
        int epoll_fd;
        std::vector<Conn>connections;
        std::atomic<bool> running;

        void acceptConnection();
        void handleConnection(int fd, uint32_t events);
        void closeConnection(int fd);

    public:
        ReddishServer(int port);
        void run();
        void shutdown();

        //when user enters Ctrl+C, shutting down the server gracefully instead of abruptly killing the process
        static void handleSignal(int signal);
        void setupSignalHandler(int signal);
};

#endif