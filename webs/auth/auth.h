#pragma once
#include <string>
#include "../json.hpp"

std::string generateToken(const std::string& username);
nlohmann::json loginUser(const nlohmann::json& body, int& status);