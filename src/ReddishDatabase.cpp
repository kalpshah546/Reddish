#include "ReddishDatabase.h"
#include <iostream>

ReddishDatabase& ReddishDatabase::getInstance(){
    static ReddishDatabase instance;
    return instance;
}

bool ReddishDatabase::dump(const std::string& filename){
    return true;
}

bool ReddishDatabase::load(const std::string& filename){
    return true;
}