#include "auth.h"
#include "../db/database.h"

using json = nlohmann::json;

std::string generateToken(const std::string& username) {
    return username + "_token";
}

json loginUser(const json& body, int& status) {

    json response;

    std::string username = body["username"];
    std::string password = body["password"];

    bool found = false;

    {
        std::lock_guard<std::mutex> lock(mtx);

        for (auto& user : users) {
            if (user.username == username && user.password == password) {
                found = true;
                break;
            }
        }
    }

    if (found) {
        std::string token = generateToken(username);

        {
            std::lock_guard<std::mutex> lock(mtx);
            sessions[token] = username;
        }

        response["token"] = token;
    }
    else {
        response["error"] = "Invalid credentials";
        status = 401;
    }

    return response;
}