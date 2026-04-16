#pragma once
#include <vector>
#include <map>
#include <string>
#include <mutex>

struct User {
    std::string username;
    std::string password;
};

extern std::vector<User> users;
extern std::map<std::string, std::string> sessions;
extern std::mutex mtx;