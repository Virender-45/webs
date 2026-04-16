#include "router.h"
#include "../db/database.h"
#include "../auth/auth.h"
#include "../utils/response.h"
#include "json.hpp"
#include <mutex>

using json = nlohmann::json;

void handleClient(SOCKET clientSocket) {

    char buffer[4096] = { 0 };
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {

        std::string requestStr(buffer, bytesReceived);
        HttpRequest req = parseRequest(requestStr);

        json response;
        int status = 200;

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
            }

            // PROFILE
            else if (req.path == "/profile" && req.method == "GET") {

                std::string token;

                if (req.headers.find("authorization") != req.headers.end()) {
                    std::string authHeader = req.headers["authorization"];

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

        std::string httpResponse = createResponse(response, status);
        send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
    }

    closesocket(clientSocket);
}