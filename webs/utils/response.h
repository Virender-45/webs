#pragma once
#include <string>
#include "../json.hpp"

std::string createResponse(const nlohmann::json& body, int status);