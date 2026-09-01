#ifndef REDDISH_DATABASE_H
#define REDDISH_DATABASE_H
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <chrono>

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
        std::unordered_map<std::string,std::chrono::steady_clock::time_point> expiry_map;

    public:
        //this is singleton instances so no other copy or assignment is allowed
        static ReddishDatabase& getInstance();
        //persistence
        bool dump(const std::string& filename);
        bool load(const std::string& filename);

        bool flushALL();
        void set(const std::string& key,const std::string& value);
        bool get(const std::string& key,std::string& value);
        std::vector<std::string> keys();
        bool del(const std::string& key);
        std::string type(const std::string& key);

        bool expire(const std::string& key,int seconds);
        bool rename(const std::string& old_key,const std::string& new_key);


};


#endif