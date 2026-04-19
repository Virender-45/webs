#include "middleware.h"

void MiddlewareManager::use(Middleware mw) {
    middlewares.push_back(mw);
}

bool MiddlewareManager::execute(HttpRequest& req, nlohmann::json& res, int& status) {

    for (auto& mw : middlewares) {
        if (!mw(req, res, status)) {
            return false; // stop chain
        }
    }

    return true;
}