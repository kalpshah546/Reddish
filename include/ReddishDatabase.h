#ifndef REDDISH_DATABASE_H
#define REDDISH_DATABASE_H
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>

class ReddishDatabase {
    private:
        ReddishDatabase()=default;
        ~ReddishDatabase()=default;
        ReddishDatabase(const ReddishDatabase&) = delete;
        ReddishDatabase& operator=(const ReddishDatabase&) = delete;
        std::mutex db_mutex;
        std::unordered_map<std::string,std::string> key_value_store;
        std::unordered_map<std::string,std::unordered_map<std::string,std::string>> hash_store;
        std::unordered_map<std::string,std::vector<std::string>> list_store;

    public:
        //this is singleton instances so no other copy or assignment is allowed
        static ReddishDatabase& getInstance();
        //persistence
        bool dump(const std::string& filename);
        bool load(const std::string& filename);


};


#endif