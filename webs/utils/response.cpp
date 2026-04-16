#include "response.h"

std::string getStatusText(int status) {
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 404: return "Not Found";
        default: return "OK";
    }
}

std::string createResponse(const json& body, int status) {

    std::string bodyStr = body.dump();

    std::string response =
        "HTTP/1.1 " + std::to_string(status) + " " + getStatusText(status) + "\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(bodyStr.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        bodyStr;

    return response;
}