#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <string>

// HTTP Request Structure
struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::string body;
};

// HTTP Parser Function
HttpRequest parseRequest(const std::string& request) {

    HttpRequest req;

    // Split headers and body
    size_t pos = request.find("\r\n\r\n");
    std::string headers = request.substr(0, pos);
    req.body = (pos != std::string::npos) ? request.substr(pos + 4) : "";

    // First line (METHOD PATH VERSION)
    size_t lineEnd = headers.find("\r\n");
    std::string firstLine = headers.substr(0, lineEnd);

    // Parse method, path, version
    size_t mEnd = firstLine.find(" ");
    size_t pEnd = firstLine.find(" ", mEnd + 1);

    req.method = firstLine.substr(0, mEnd);
    req.path = firstLine.substr(mEnd + 1, pEnd - mEnd - 1);
    req.version = firstLine.substr(pEnd + 1);

    return req;
}

// Handle each client (MULTITHREADING)
void handleClient(SOCKET clientSocket) {

    char buffer[4096] = { 0 };

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {

        std::string requestStr(buffer, bytesReceived);

        // Parse request
        HttpRequest req = parseRequest(requestStr);

        std::cout << "\n--- Parsed Request ---\n";
        std::cout << "Method: " << req.method << std::endl;
        std::cout << "Path: " << req.path << std::endl;
        std::cout << "Body: " << req.body << std::endl;

        // Basic Routing
        std::string responseBody;

        if (req.path == "/") {
            responseBody = "Welcome to C++ Server";
        }
        else if (req.path == "/login") {
            responseBody = "Login endpoint hit";
        }
        else {
            responseBody = "404 Not Found";
        }

        // HTTP Response
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(responseBody.size()) + "\r\n"
            "\r\n" +
            responseBody;

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    closesocket(clientSocket);
}

int main() {

    // 1) Initialize Winsock
    WSADATA wsaData;
    WORD version = MAKEWORD(2, 2);

    if (WSAStartup(version, &wsaData) != 0) {
        std::cout << "Winsock init failed\n";
        return 1;
    }

    // 2) Create socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    // 3) Bind
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 4) Listen
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "🚀 Server running on http://localhost:54000\n";

    // 5) Accept loop (MULTITHREADED)
    while (true) {

        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Accept failed\n";
            continue;
        }

        std::cout << "Client connected!\n";

        // Create thread
        std::thread t(handleClient, clientSocket);
        t.detach();
    }

    // Cleanup (never reached)
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}