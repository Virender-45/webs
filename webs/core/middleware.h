#pragma once
#include <functional>
#include <vector>
#include "../http/parserr.h"
#include "../json.hpp"


using Middleware = std::function<bool(HttpRequest&, nlohmann::json&, int&)>;

class MiddlewareManager {
private:
    std::vector<Middleware> middlewares;

public:
    void use(Middleware mw);
    bool execute(HttpRequest& req, nlohmann::json& res, int& status);
};