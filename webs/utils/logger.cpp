#include "logger.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>
#include <filesystem>

std::mutex logMutex;
std::ofstream logFile;

const std::string LOG_FILE = "server.log";
const size_t MAX_LOG_SIZE = 1024 * 1024; // 1 MB

std::string getTime() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    ctime_s(buf, sizeof(buf), &now);
    buf[strlen(buf) - 1] = '\0';
    return std::string(buf);
}

void rotateLogs() {

    if (!std::filesystem::exists(LOG_FILE)) return;

    auto size = std::filesystem::file_size(LOG_FILE);

    if (size >= MAX_LOG_SIZE) {
        std::filesystem::rename(LOG_FILE, "server_old.log");
    }
}

void initLogger() {
    std::lock_guard<std::mutex> lock(logMutex);

    rotateLogs();

    logFile.open(LOG_FILE, std::ios::app);
}

void writeLog(const std::string& level, const std::string& message) {

    std::lock_guard<std::mutex> lock(logMutex);

    std::string logLine = "[" + level + "] [" + getTime() + "] " + message;

    // Console
    if (level == "ERROR")
        std::cerr << logLine << std::endl;
    else
        std::cout << logLine << std::endl;

    // File
    if (logFile.is_open()) {
        logFile << logLine << std::endl;
    }
}

void logInfo(const std::string& message) {
    writeLog("INFO", message);
}

void logError(const std::string& message) {
    writeLog("ERROR", message);
}