#include "static.h"
#include <fstream>
#include <sstream>

// Detect file type
std::string getContentType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos) return "image/jpeg";
    if (path.find(".jpeg") != std::string::npos) return "image/jpeg";
    return "text/plain";
}

std::string serveStaticFile(const std::string& path, std::string& contentType, int& status) {

    // Default route
    std::string fullPath = "public" + path;

    if (path == "/") {
        fullPath = "public/index.html";
    }

    std::ifstream file(fullPath, std::ios::binary);

    if (!file.is_open()) {
        status = 404;
        return "File not found";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    contentType = getContentType(path);
    status = 200;

    return buffer.str();
}