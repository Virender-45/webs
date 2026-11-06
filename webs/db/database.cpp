
#include "database.h"

#include <mysql/jdbc.h>
#include <iostream>

Database::Database() = default;

Database::~Database() {
    disconnect();
}

bool Database::connect(
    const std::string& host,
    const std::string& username,
    const std::string& password,
    const std::string& databaseName
) {
    try {
        sql::Driver* driver = sql::mysql::get_driver_instance();

        connection.reset(
            driver->connect(host, username, password)
        );

        connection->setSchema(databaseName);

        std::cout << "MySQL connected successfully!"
            << std::endl;

        return true;
    }
    catch (const sql::SQLException& e) {
        std::cerr << "MySQL connection failed: "
            << e.what()
            << "\nError code: " << e.getErrorCode()
            << "\nSQL State: " << e.getSQLState()
            << std::endl;

        connection.reset();
        return false;
    }
}

bool Database::isConnected() const {
    try {
        return connection && connection->isValid();
    }
    catch (...) {
        return false;
    }
}

void Database::disconnect() {
    if (connection) {
        connection->close();
        connection.reset();

        std::cout << "MySQL disconnected."
            << std::endl;
    }
}













//#include "database.h"
//
//std::vector<User> users;
//std::map<std::string, std::string> sessions;
//std::mutex mtx;