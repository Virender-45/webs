#include "auth.h"

std::string generateToken(const std::string& username) {
    return username + "_token";
}