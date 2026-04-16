#pragma once
#include <string>
#include "json.hpp"

using json = nlohmann::json;

std::string getStatusText(int status);
std::string createResponse(const json& body, int status = 200);