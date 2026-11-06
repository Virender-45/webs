
#ifndef DATABASE_H
#define DATABASE_H

#include <memory>
#include <string>

namespace sql {
    class Connection;
}

class Database {
private:
    std::unique_ptr<sql::Connection> connection;

public:
    Database();
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool connect(
        const std::string& host,
        const std::string& username,
        const std::string& password,
        const std::string& databaseName
    );

    bool isConnected() const;
    void disconnect();
};

#endif







//#pragma once
//#include <vector>
//#include <map>
//#include <string>
//#include <mutex>
//
//struct User {
//    std::string username;
//    std::string password;
//};
//
//extern std::vector<User> users;
//extern std::map<std::string, std::string> sessions;
//extern std::mutex mtx;