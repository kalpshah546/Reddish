#include "ReddishCommandHandler.h"
#include "ReddishDatabase.h"
#include <vector>
#include <sstream>
#include <algorithm>

std::vector<std::string> parseCommand(const std::string& input){
    std::vector<std::string>tokens;
    if(input.size()==0) return tokens;
    if(input[0]!='*'){
        std::istringstream iss(input);
        std::string token;
        while(iss>>token){
            tokens.push_back(token);
        }
        return tokens;
    }
        size_t pos=0;
        if(input[pos]!='*') return tokens;
        pos++;
        size_t crlf=input.find("\r\n",pos);
        // cr=carriage return lf=line feed
        if(crlf==std::string::npos) return tokens;
        int totalElements=std::stoi(input.substr(pos,crlf-pos));
        pos=crlf+2;
        for(int i=0;i<totalElements;i++){
            if(pos>=input.size() || input[pos]!='$') break;
            pos++; //skip $ in the string
            crlf=input.find("\r\n",pos);
            if(crlf==std::string::npos) break;
            int len=std::stoi(input.substr(pos,crlf-pos));
            pos=crlf+2;
            if(pos+len>input.size()) break;
            tokens.push_back(input.substr(pos,len));
            pos+=len+2;
        }
        return tokens;

    
}
ReddishCommandHandler::ReddishCommandHandler(){}
std::string ReddishCommandHandler::processCommand(const std::string& commandLine){
    auto tokens=parseCommand(commandLine);
    if(tokens.empty()){
        return "-ERR Empty command\r\n";
    }
    std::string command=tokens[0];
    std::transform(command.begin(),command.end(),command.begin(),::toupper);
    std::ostringstream response;
    ReddishDatabase& db=ReddishDatabase::getInstance();
    if(command=="PING"){
        response<<"+PONG\r\n";
    }
    else if(command=="ECHO"){

    }
}
