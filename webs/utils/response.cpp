#include "response.h"
#include "../json.hpp"

using json = nlohmann::json;

std::string getStatusText(int status) {
    switch (status) {
    case 200: return "OK";
    case 201: return "Created";
    case 400: return "Bad Request";
    case 401: return "Unauthorized";
    case 404: return "Not Found";
    case 429: return "Too Many Requests";
    default: return "OK";
    }
}

std::string createResponse(const json& body, int status) {

    //HANDLE STATIC FILE RESPONSE
    if (body.contains("__raw__")) {

        std::string content = body["__raw__"].get<std::string>();
        std::string contentType = body["contentType"].get<std::string>();

        return "HTTP/1.1 " + std::to_string(status) + " " + getStatusText(status) + "\r\n"
            "Content-Type: " + contentType + "\r\n"
            "Content-Length: " + std::to_string(content.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + content;
    }

    //NORMAL JSON RESPONSE
    std::string bodyStr = body.dump();

    return "HTTP/1.1 " + std::to_string(status) + " " + getStatusText(status) + "\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(bodyStr.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + bodyStr;
}