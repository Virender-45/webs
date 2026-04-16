#pragma once
#include "../json.hpp"
#include <string>

bool validateUserInput(const nlohmann::json& body, std::string& error);