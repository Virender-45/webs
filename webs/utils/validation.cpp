#include "validation.h"

using json = nlohmann::json;

bool validateUserInput(const json& body, std::string& error) {

    if (!body.contains("username") || !body["username"].is_string()) {
        error = "Username is required and must be string";
        return false;
    }

    if (!body.contains("password") || !body["password"].is_string()) {
        error = "Password is required and must be string";
        return false;
    }

    if (body["username"].get<std::string>().length() < 3) {
        error = "Username too short";
        return false;
    }

    if (body["password"].get<std::string>().length() < 3) {
        error = "Password too short";
        return false;
    }

    return true;
}