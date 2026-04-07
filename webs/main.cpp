#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include "json.hpp"



using json = nlohmann::json;

//https://github.com/nlohmann/json/tree/develop/include/nlohmann

// 🔐 Database
struct User {
    std::string username;
    std::string password;
};

std::vector<User> users;
std::map<std::string, std::string> sessions;
std::mutex mtx;

// 🔹 HTTP Request
struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
};

// 🔹 Parse HTTP
HttpRequest parseRequest(const std::string& request) {
    HttpRequest req;

    size_t pos = request.find("\r\n\r\n");
    std::string headers = request.substr(0, pos);
    req.body = (pos != std::string::npos) ? request.substr(pos + 4) : "";

    size_t lineEnd = headers.find("\r\n");
    std::string firstLine = headers.substr(0, lineEnd);

    size_t mEnd = firstLine.find(" ");
    size_t pEnd = firstLine.find(" ", mEnd + 1);

    req.method = firstLine.substr(0, mEnd);
    req.path = firstLine.substr(mEnd + 1, pEnd - mEnd - 1);

    return req;
}

// 🔑 Generate Token (simple)
std::string generateToken(const std::string& username) {
    return username + "_token";
}

// 🔹 Create HTTP response
std::string createResponse(const json& body, int status = 200) {
    std::string bodyStr = body.dump();

    return "HTTP/1.1 " + std::to_string(status) + " OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(bodyStr.size()) + "\r\n"
        "\r\n" + bodyStr;
}

// 🔹 Handle client
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

            // 🔹 REGISTER USER
            if (req.path == "/users" && req.method == "POST") {

                std::lock_guard<std::mutex> lock(mtx);

                User newUser{ body["username"], body["password"] };
                users.push_back(newUser);

                response["message"] = "User created";
            }

            // 🔹 LOGIN
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

            // 🔹 PROTECTED ROUTE
            else if (req.path == "/profile" && req.method == "GET") {

                // VERY SIMPLE TOKEN CHECK (improve later)
                if (requestStr.find("Authorization: Bearer ") != std::string::npos) {

                    size_t pos = requestStr.find("Authorization: Bearer ") + 22;
                    std::string token = requestStr.substr(pos, 20);

                    std::lock_guard<std::mutex> lock(mtx);

                    if (sessions.find(token) != sessions.end()) {
                        response["user"] = sessions[token];
                    }
                    else {
                        response["error"] = "Invalid token";
                        status = 401;
                    }

                }
                else {
                    response["error"] = "No token provided";
                    status = 401;
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

int main() {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    std::cout << "🚀 API Server running on http://localhost:54000\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

        std::thread t(handleClient, clientSocket);
        t.detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}