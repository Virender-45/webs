#pragma once
#include <string>

// Returns file content + sets content type + status
std::string serveStaticFile(const std::string& path, std::string& contentType, int& status);