#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {

    //Setting up the server
	//1) Initiqlize Winsock
    
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    if (wsaerr != 0) {
        std::cout << "The Winsock dll not found!" << std::endl;
        return 0;
    }
    else {
        std::cout << "The Winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

	//2) Create a socket
    SOCKET serverSocket;
    serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "Socket is OK!" << std::endl;
    }

	//3) Bind the socket
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(54000);
    service.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "bind() is OK!" << std::endl;
    }

	//4) Listen for connections
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "listen() is OK! I'm waiting for new connections..." << std::endl;
    }

    //5) Accept connections
    SOCKET acceptSocket;
    acceptSocket = accept(serverSocket, nullptr, nullptr);

    if (acceptSocket == INVALID_SOCKET) {
        std::cout << "accept failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }
    else {
        std::cout << "accept() is OK!" << std::endl;
    }

    //6) Receive and Send Data
    //Recieve
    char receiveBuffer[200];
    int rbyteCount = recv(acceptSocket, receiveBuffer, 200, 0);

    if (rbyteCount < 0) {
        std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
        return 0;
    }
    else {
        std::cout << "Received data: " << receiveBuffer << std::endl;
    }

    //Send
    char buffer[200];
    std::cout << "Enter the message: ";
    std::cin.getline(buffer, 200);
    int sbyteCount = send(acceptSocket, buffer, 200, 0);

    if (sbyteCount == SOCKET_ERROR) {
        std::cout << "Server send error: " << WSAGetLastError() << std::endl;
        return -1;
    }
    else {
        std::cout << "Server: Sent " << sbyteCount << " bytes" << std::endl;
    }


    return 0;
}