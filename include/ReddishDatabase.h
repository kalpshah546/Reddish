#ifndef REDDISH_DATABASE_H
#define REDDISH_DATABASE_H
#include <string>
class ReddishDatabase {
    private:
        ReddishDatabase()=default;
        ~ReddishDatabase()=default;
        ReddishDatabase(const ReddishDatabase&) = delete;
        ReddishDatabase& operator=(const ReddishDatabase&) = delete;
    public:
        //this is singleton instances so no other copy or assignment is allowed
        static ReddishDatabase& getInstance();
        //persistence
        bool dump(const std::string& filename);
        bool load(const std::string& filename);
};


#endif