#include "database.h"

std::vector<User> users;
std::map<std::string, std::string> sessions;
std::mutex mtx;