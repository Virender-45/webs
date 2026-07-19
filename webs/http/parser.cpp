#include "parserr.h"
#include <sstream>
#include <algorithm>

HttpRequest parseRequest(const std::string& request)
{
    HttpRequest req;

    size_t pos = request.find("\r\n\r\n");

    std::string headerPart =
        (pos == std::string::npos)
        ? request
        : request.substr(0, pos);

    req.body =
        (pos == std::string::npos)
        ? ""
        : request.substr(pos + 4);

    std::istringstream stream(headerPart);

    std::string line;

    if (!std::getline(stream, line))
        return req;

    if (!line.empty() && line.back() == '\r')
        line.pop_back();

    std::istringstream firstLine(line);
    firstLine >> req.method >> req.path >> req.version;

    while (std::getline(stream, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty())
            break;

        size_t colon = line.find(": ");

        if (colon == std::string::npos)
            continue;

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 2);

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);

        req.headers[key] = value;
    }

    return req;
}