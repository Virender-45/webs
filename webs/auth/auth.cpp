#include "auth.h"
#include "../db/database.h"
#include "../utils/hash.h"

using json = nlohmann::json;

std::string generateToken(const std::string& username) {
    return generateRandomToken();
}

json loginUser(const json& body, int& status) {

    json response;

    if (!body.contains("username") || !body.contains("password")) {
        response["error"] = "Missing username or password";
        status = 400;
        return response;
    }

    if (!body["username"].is_string() || !body["password"].is_string()) {
        response["error"] = "Invalid input types";
        status = 400;
        return response;
    }

    std::string username = body["username"];
    std::string password = body["password"];

    // HASH INPUT PASSWORD
    std::string hashedInput = hashPassword(password);

    bool found = false;

    {
        std::lock_guard<std::mutex> lock(mtx);

        for (auto& user : users) {
            if (user.username == username && user.password == hashedInput) {
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

        response["access_token"] = token;
        response["token_type"] = "Bearer";
    }
    else {
        response["error"] = "Invalid credentials";
        status = 401;
    }

    return response;
}