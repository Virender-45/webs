#include "static.h"
#include <fstream>
#include <sstream>

// ================= CONTENT TYPE =================
std::string getContentType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos) return "image/jpeg";
    if (path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".svg") != std::string::npos) return "image/svg+xml";
    if (path.find(".ico") != std::string::npos) return "image/x-icon";
    if (path.find(".json") != std::string::npos) return "application/json";
    return "text/plain";
}

// ================= STATIC FILE SERVER =================
std::string serveStaticFile(const std::string& path, std::string& contentType, int& status) {

    std::string fullPath;

    // Handle root
    if (path == "/" || path.empty()) {
        fullPath = "public/index.html";
    }
    else {
        fullPath = "public" + path;
    }

    // Prevent path traversal (security)
    if (fullPath.find("..") != std::string::npos) {
        status = 403;
        return "";
    }

    std::ifstream file(fullPath, std::ios::binary);

    // File not found
    if (!file.is_open()) {
        status = 404;

        // OPTIONAL: SPA fallback (serve index.html)
        std::ifstream fallback("public/index.html", std::ios::binary);
        if (fallback.is_open()) {
            std::stringstream buffer;
            buffer << fallback.rdbuf();
            contentType = "text/html";
            status = 200;
            return buffer.str();
        }

        return "";
    }

    // Read file
    std::stringstream buffer;
    buffer << file.rdbuf();

    // IMPORTANT: use fullPath (not path)
    contentType = getContentType(fullPath);
    status = 200;

    return buffer.str();
}