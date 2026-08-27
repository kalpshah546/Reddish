#include "ReddishDatabase.h"
#include <iostream>
#include <mutex>
#include <fstream>
#include <sstream>

ReddishDatabase& ReddishDatabase::getInstance(){
    static ReddishDatabase instance;
    return instance;
}

bool ReddishDatabase::dump(const std::string& filename){
    std::lock_guard<std::mutex> lock(db_mutex);
    std::ofstream ofs(filename,std::ios::binary);
    if(!ofs){
        std::cerr<<"Failed to open file "<<filename<<" for writing"<<std::endl;
        return false;
    }
    for(const auto& pair:key_value_store){
        ofs<<"K"<<" "<<pair.first.size()<<" "<<pair.first<<" "<<pair.second.size()<<" "<<pair.second<<"\n";
    }
    for(const auto& pair:hash_store){
        ofs<<"H"<<" "<<pair.first.size()<<" "<<pair.first<<" "<<pair.second.size()<<"\n";
        for(const auto& inner_pair:pair.second){
            ofs<<inner_pair.first.size()<<" "<<inner_pair.first<<" "<<inner_pair.second.size()<<" "<<inner_pair.second<<"\n";
        }
    }
    for(const auto& pair:list_store){
        ofs<<"L"<<" "<<pair.first.size()<<" "<<pair.first<<" "<<pair.second.size()<<"\n";
        for(const auto& value:pair.second){
            ofs<<value.size()<<" "<<value<<"\n";
        }
    }
    return true;
}

bool ReddishDatabase::load(const std::string& filename){
    std::lock_guard<std::mutex> lock(db_mutex);
    std::ifstream ifs(filename,std::ios::binary);
    if(!ifs){
        std::cerr<<"Failed to open file "<<filename<<" for reading"<<std::endl;
        return false;
    }
    key_value_store.clear();
    hash_store.clear();
    list_store.clear();
    std::string line;
    while(std::getline(ifs,line)){
        std::istringstream iss(line,std::ios::binary);
        char type;
        iss>>type;
        if(type=='K'){
            size_t key_size,value_size;
            std::string key,value;
            iss>>key_size>>key>>value_size>>value;
            key_value_store[key]=value;
        }
        else if(type=='H'){
            size_t key_size,hash_size;
            std::string key;
            iss>>key_size>>key>>hash_size;
            std::unordered_map<std::string,std::string> hash_map;
            for(size_t i=0;i<hash_size;i++){
                size_t field_size,value_size;
                std::string field,value;
                if(!std::getline(ifs,line)){
                    std::cerr<<"Unexpected end of file while reading hash"<<std::endl;
                    return false;
                }
                std::istringstream field_iss(line);
                field_iss>>field_size>>field>>value_size>>value;
                hash_map[field]=value;
            }
            hash_store[key]=hash_map;
        }
        else if(type=='L'){
            size_t key_size,list_size;
            std::string key;
            iss>>key_size>>key>>list_size;
            std::vector<std::string> list_values;
            for(size_t i=0;i<list_size;i++){
                size_t value_size;
                std::string value;
                if(!std::getline(ifs,line)){
                    std::cerr<<"Unexpected end of file while reading list"<<std::endl;
                    return false;
                }
                std::istringstream value_iss(line);
                value_iss>>value_size>>value;
                list_values.push_back(value);
            }
            list_store[key]=list_values;
        }
    }
    return true;
}