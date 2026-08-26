#ifndef REDDISH_COMMAND_HANDLER_H
#define REDDISH_COMMAND_HANDLER_H

#include <string>
class ReddishCommandHandler{
    public:
    ReddishCommandHandler();
    std::string processCommand(const std:: string&commandline);
};


#endif