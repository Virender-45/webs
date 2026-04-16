#include "hash.h"
#include <sstream>
#include <iomanip>
#include <random>

// Simple hash (for learning)
std::string hashPassword(const std::string& password) {

    std::hash<std::string> hasher;
    size_t hashed = hasher(password);

    std::stringstream ss;
    ss << std::hex << hashed;

    return ss.str();
}

// Generate random token
std::string generateRandomToken() {

    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string token;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    for (int i = 0; i < 32; i++) {
        token += charset[dist(gen)];
    }

    return token;
}