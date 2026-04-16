#include "router.h"
#include "../db/database.h"
#include "../auth/auth.h"

using json = nlohmann::json;

json handleRoute(const HttpRequest& req, int& status) {

    json response;

    try {
        json body = req.body.empty() ? json{} : json::parse(req.body);

        // REGISTER
        if (req.path == "/users" && req.method == "POST") {

            std::lock_guard<std::mutex> lock(mtx);

            User newUser{ body["username"], body["password"] };
            users.push_back(newUser);

            response["message"] = "User created";
            status = 201;
        }

        // LOGIN
        else if (req.path == "/login" && req.method == "POST") {
            return loginUser(body, status);
        }

        // PROFILE
        else if (req.path == "/profile" && req.method == "GET") {

            std::string token;

            if (req.headers.find("authorization") != req.headers.end()) {
                std::string authHeader = req.headers.at("authorization");

                if (authHeader.find("Bearer ") == 0) {
                    token = authHeader.substr(7);
                }
            }

            if (token.empty()) {
                response["error"] = "No token provided";
                status = 401;
            }
            else {
                std::lock_guard<std::mutex> lock(mtx);

                if (sessions.find(token) != sessions.end()) {
                    response["user"] = sessions[token];
                }
                else {
                    response["error"] = "Invalid token";
                    status = 401;
                }
            }
        }

        else {
            response["error"] = "Not Found";
            status = 404;
        }

    }
    catch (...) {
        response["error"] = "Invalid JSON";
        status = 400;
    }

    return response;
}