#pragma once
#include <string>
#include <map>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::string body;
    std::map<std::string, std::string> headers;
};

HttpRequest parseRequest(const std::string& request);