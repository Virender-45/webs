#pragma once
#include "../http/parserr.h"
#include "../json.hpp"

nlohmann::json handleRoute(const HttpRequest& req, int& status);