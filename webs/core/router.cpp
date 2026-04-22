#include "router.h"
#include "../db/database.h"
#include "../auth/auth.h"
#include "../utils/validation.h"
#include "../utils/hash.h"
#include "../core/static.h"
#include <fstream>

using json = nlohmann::json;

json handleRoute(const HttpRequest& req, int& status) {

    json response;

    try {
        json body;

        if (!req.body.empty()) {
            try {
                body = json::parse(req.body);
            }
            catch (...) {
                response["error"] = "Invalid JSON format";
                status = 400;
                return response;
            }
        }

        // ================= STATIC FILE HANDLER =================
        if (req.method == "GET") {

            std::string contentType;
            std::string content = serveStaticFile(req.path, contentType, status);

            if (status == 200) {
                json res;
                res["__raw__"] = content;
                res["contentType"] = contentType;
                return res;
            }
        }

        // ================= API ROUTES =================

        // REGISTER
        if (req.path == "/users" && req.method == "POST") {

            std::string error;
            if (!validateUserInput(body, error)) {
                response["error"] = error;
                status = 400;
                return response;
            }

            std::lock_guard<std::mutex> lock(mtx);

            std::string hashedPassword = hashPassword(body["password"]);

            User newUser{ body["username"], hashedPassword };
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

        // CONTACT FORM (ADDED HERE)
        else if (req.path == "/contact" && req.method == "POST") {

            if (!body.contains("name") || !body.contains("email") || !body.contains("message")) {
                response["error"] = "Missing fields";
                status = 400;
                return response;
            }

            std::ofstream file("messages.txt", std::ios::app);

            if (!file.is_open()) {
                response["error"] = "Failed to save message";
                status = 500;
                return response;
            }

            file << "Name: " << body["name"] << "\n";
            file << "Email: " << body["email"] << "\n";
            file << "Subject: " << body["subject"] << "\n";
            file << "Message: " << body["message"] << "\n";
            file << "-----------------------------\n";

            file.close();

            response["message"] = "Message saved";
            status = 200;
        }

        // ================= NOT FOUND =================
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