#include "logger.h"
#include <iostream>
#include <mutex>
#include <ctime>

std::mutex logMutex;

std::string getTime() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    ctime_s(buf, sizeof(buf), &now);
    buf[strlen(buf) - 1] = '\0'; // remove newline
    return std::string(buf);
}

void logInfo(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << "[INFO] [" << getTime() << "] " << message << std::endl;
}

void logError(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cerr << "[ERROR] [" << getTime() << "] " << message << std::endl;
}