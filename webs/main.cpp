#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#include "http/parserr.h"
#include "core/router.h"
#include "utils/response.h"
#include "core/threadpool.h"
#include "utils/logger.h"
#include "core/ratelimiter.h"
#include "core/middleware.h"

// GLOBAL MIDDLEWARE MANAGER
MiddlewareManager middlewareManager;

// ================= CLIENT HANDLER =================
void handleClient(SOCKET clientSocket, std::string clientIP) {

    char buffer[4096] = { 0 };
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {

        std::string requestStr(buffer, bytesReceived);

        HttpRequest req = parseRequest(requestStr);

        // Attach IP for middleware usage
        req.headers["x-forwarded-for"] = clientIP;

        nlohmann::json response;
        int status = 200;

        // RUN MIDDLEWARE
        if (!middlewareManager.execute(req, response, status)) {

            std::string httpResponse = createResponse(response, status);
            send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
            closesocket(clientSocket);
            return;
        }

        // ROUTING
        response = handleRoute(req, status);

        std::string httpResponse = createResponse(response, status);
        send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
    }

    closesocket(clientSocket);
}

// ================= MAIN =================
int main() {

    initLogger();
    logInfo("Server starting...");

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    logInfo("Server started on port 54000");

    // ================= MIDDLEWARE REGISTRATION =================

    // Logging Middleware
    middlewareManager.use([](HttpRequest& req, nlohmann::json& res, int& status) {
        logInfo(req.method + " " + req.path);
        return true;
        });

    // Auth Middleware
    middlewareManager.use([](HttpRequest& req, nlohmann::json& res, int& status) {

        if (req.path == "/profile") {

            if (req.headers.find("authorization") == req.headers.end()) {
                res["error"] = "Unauthorized";
                status = 401;
                return false;
            }
        }

        return true;
        });

    // Rate Limiter Middleware
    middlewareManager.use([](HttpRequest& req, nlohmann::json& res, int& status) {

        std::string ip = req.headers.count("x-forwarded-for")
            ? req.headers["x-forwarded-for"]
            : "unknown";

        if (isRateLimited(ip)) {
            res["error"] = "Too many requests";
            status = 429;
            return false;
        }

        return true;
        });

    // ================= THREAD POOL =================
    ThreadPool pool(4);

    // ================= SERVER LOOP =================
    while (true) {

        sockaddr_in clientAddr;
        int addrSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrSize);

        if (clientSocket == INVALID_SOCKET) {
            logError("Accept failed");
            continue;
        }

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, sizeof(ipStr));

        std::string clientIP = ipStr;

        pool.enqueue(clientSocket, clientIP);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

















//main code before separation

// #pragma comment(lib, "ws2_32.lib")

// #include <iostream>
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <thread>
// #include <string>
// #include <vector>
// #include <map>
// #include <mutex>
// #include <sstream>
// #include <algorithm>

// #include "json.hpp"

// using json = nlohmann::json;

// // ================= DATABASE =================
// struct User {
//     std::string username;
//     std::string password;
// };

// std::vector<User> users;
// std::map<std::string, std::string> sessions;
// std::mutex mtx;

// // ================= HTTP REQUEST =================
// struct HttpRequest {
//     std::string method;
//     std::string path;
//     std::string version;
//     std::string body;
//     std::map<std::string, std::string> headers;
// };

// // ================= PARSE REQUEST =================
// HttpRequest parseRequest(const std::string& request) {

//     HttpRequest req;

//     size_t pos = request.find("\r\n\r\n");
//     std::string headerPart = request.substr(0, pos);
//     req.body = (pos != std::string::npos) ? request.substr(pos + 4) : "";

//     std::istringstream stream(headerPart);
//     std::string line;

//     std::getline(stream, line);
//     line.pop_back();

//     std::istringstream firstLine(line);
//     firstLine >> req.method >> req.path >> req.version;

//     while (std::getline(stream, line) && line != "\r") {
//         line.pop_back();

//         size_t colon = line.find(": ");
//         if (colon != std::string::npos) {
//             std::string key = line.substr(0, colon);
//             std::string value = line.substr(colon + 2);

//             std::transform(key.begin(), key.end(), key.begin(), ::tolower);
//             req.headers[key] = value;
//         }
//     }

//     return req;
// }

// // ================= TOKEN =================
// std::string generateToken(const std::string& username) {
//     return username + "_token";
// }

// // ================= STATUS TEXT =================
// std::string getStatusText(int status) {
//     switch (status) {
//     case 200: return "OK";
//     case 201: return "Created";
//     case 400: return "Bad Request";
//     case 401: return "Unauthorized";
//     case 404: return "Not Found";
//     default: return "OK";
//     }
// }

// // ================= RESPONSE =================
// std::string createResponse(const json& body, int status = 200) {

//     std::string bodyStr = body.dump();

//     std::string response =
//         "HTTP/1.1 " + std::to_string(status) + " " + getStatusText(status) + "\r\n"
//         "Content-Type: application/json\r\n"
//         "Content-Length: " + std::to_string(bodyStr.size()) + "\r\n"
//         "Connection: close\r\n"
//         "\r\n" +
//         bodyStr;

//     return response;
// }

// // ================= CLIENT HANDLER =================
// void handleClient(SOCKET clientSocket) {

//     char buffer[4096] = { 0 };
//     int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

//     if (bytesReceived > 0) {

//         std::string requestStr(buffer, bytesReceived);
//         HttpRequest req = parseRequest(requestStr);

//         json response;
//         int status = 200;

//         try {
//             json body = req.body.empty() ? json{} : json::parse(req.body);

//             // ========== REGISTER ==========
//             if (req.path == "/users" && req.method == "POST") {

//                 std::lock_guard<std::mutex> lock(mtx);

//                 User newUser{ body["username"], body["password"] };
//                 users.push_back(newUser);

//                 response["message"] = "User created";
//                 status = 201;  // ✅ ADDED
//             }

//             // ========== LOGIN ==========
//             else if (req.path == "/login" && req.method == "POST") {

//                 std::string username = body["username"];
//                 std::string password = body["password"];

//                 bool found = false;

//                 {
//                     std::lock_guard<std::mutex> lock(mtx);

//                     for (auto& user : users) {
//                         if (user.username == username && user.password == password) {
//                             found = true;
//                             break;
//                         }
//                     }
//                 }

//                 if (found) {
//                     std::string token = generateToken(username);

//                     {
//                         std::lock_guard<std::mutex> lock(mtx);
//                         sessions[token] = username;
//                     }

//                     response["token"] = token;
//                 }
//                 else {
//                     response["error"] = "Invalid credentials";
//                     status = 401;
//                 }
//             }

//             // ========== PROTECTED ==========
//             else if (req.path == "/profile" && req.method == "GET") {

//                 std::string token;

//                 if (req.headers.find("authorization") != req.headers.end()) {
//                     std::string authHeader = req.headers["authorization"];

//                     if (authHeader.find("Bearer ") == 0) {
//                         token = authHeader.substr(7);
//                     }
//                 }

//                 if (token.empty()) {
//                     response["error"] = "No token provided";
//                     status = 401;
//                 }
//                 else {
//                     std::lock_guard<std::mutex> lock(mtx);

//                     if (sessions.find(token) != sessions.end()) {
//                         response["user"] = sessions[token];
//                     }
//                     else {
//                         response["error"] = "Invalid token";
//                         status = 401;
//                     }
//                 }
//             }

//             // ========== NOT FOUND ==========
//             else {
//                 response["error"] = "Not Found";
//                 status = 404;
//             }

//         }
//         catch (...) {
//             response["error"] = "Invalid JSON";
//             status = 400;
//         }

//         std::string httpResponse = createResponse(response, status);
//         send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
//     }

//     closesocket(clientSocket);
// }

// // ================= MAIN =================
// int main() {

//     WSADATA wsaData;
//     WSAStartup(MAKEWORD(2, 2), &wsaData);

//     SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

//     sockaddr_in serverAddr;
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(54000);
//     serverAddr.sin_addr.s_addr = INADDR_ANY;

//     bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
//     listen(serverSocket, SOMAXCONN);

//     std::cout << "API Server running on http://localhost:54000\n";

//     while (true) {
//         SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

//         std::thread t(handleClient, clientSocket);
//         t.detach();
//     }

//     closesocket(serverSocket);
//     WSACleanup();

//     return 0;
// }