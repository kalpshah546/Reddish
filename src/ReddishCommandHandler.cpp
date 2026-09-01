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
        if(tokens.size()<2){
            response<<"-ERR ECHO command requires an argument\r\n";
        }
        else{
            response<<"+"<<tokens[1]<<"\r\n";
        }
    }
    else if(command=="FLUSHALL"){
        db.flushALL();
        response<<"+OK\r\n";
    }
    else if(command=="SET"){
        if(tokens.size()<3){
            response<<"-ERR SET command requires a key and a value\r\n";
        }
        else{
            db.set(tokens[1],tokens[2]);
            response<<"+OK\r\n";
        }
    }
    else if(command=="GET"){
        if(tokens.size()<2){
            response<<"-ERR GET command requires a key\r\n";
        }
        else{
            std::string value;
            if(db.get(tokens[1],value)){
                response<<"$"<<value.size()<<"\r\n"<<value<<"\r\n";
            }
            else{
                response<<"$-1\r\n";
            }
        }
    }
    else if(command=="KEYS"){
        auto keys=db.keys();
        response<<"*"<<keys.size()<<"\r\n";
        for(const auto& key:keys){
            response<<"$"<<key.size()<<"\r\n"<<key<<"\r\n";
        }
    }
    else if(command=="DEL"){
        if(tokens.size()<2){
            response<<"-ERR DEL command requires a key\r\n";
        }
        else{
            if(db.del(tokens[1])){
                response<<":1\r\n"; //1 means key was deleted
            }
            else{
                response<<":0\r\n"; //0 means key was not found
            }
        }
    }
    else if(command=="TYPE"){
        if(tokens.size()<2){
            response<<"-ERR TYPE command requires a key\r\n";
        }
        else{
            std::string type=db.type(tokens[1]);
            response<<"+"<<type<<"\r\n";
        }
    }
    else if(command=="EXPIRE"){
        if(tokens.size()<3){
            response<<"-ERR EXPIRE command requires a key and seconds\r\n";
        }
        else{
            int seconds=std::stoi(tokens[2]);
            if(db.expire(tokens[1],seconds)){
                response<<":1\r\n"; //1 means expiration was set
            }
            else{
                response<<":0\r\n"; //0 means key was not found
            }
        }
    }
    else if(command=="RENAME"){
        if(tokens.size()<3){
            response<<"-ERR RENAME command requires old_key and new_key\r\n";
        }
        else{
            if(db.rename(tokens[1],tokens[2])){
                response<<"+OK\r\n"; //rename successful
            }
            else{
                response<<"-ERR Key not found\r\n"; //key not found
            }
        }
    }
    else{
        response<<"-ERR Unknown command\r\n";
    }
    return response.str();
}
