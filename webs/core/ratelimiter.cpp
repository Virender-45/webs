#include "ratelimiter.h"
#include <map>
#include <vector>
#include <chrono>
#include <mutex>

std::map<std::string, std::vector<std::chrono::steady_clock::time_point>> requestLog;
std::mutex rateMutex;

const int MAX_REQUESTS = 10;
const int TIME_WINDOW = 10; // seconds

bool isRateLimited(const std::string& ip) {

    using namespace std::chrono;

    auto now = steady_clock::now();

    std::lock_guard<std::mutex> lock(rateMutex);

    auto& timestamps = requestLog[ip];

    // Remove old requests
    timestamps.erase(
        std::remove_if(timestamps.begin(), timestamps.end(),
            [&](steady_clock::time_point t) {
                return duration_cast<seconds>(now - t).count() > TIME_WINDOW;
            }),
        timestamps.end()
    );

    if (timestamps.size() >= MAX_REQUESTS) {
        return true;
    }

    timestamps.push_back(now);
    return false;
}