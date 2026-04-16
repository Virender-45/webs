#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <winsock2.h>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::pair<SOCKET, std::string>> tasks;

    std::mutex mtx;
    std::condition_variable cv;
    bool stop;

    void worker();

public:
    ThreadPool(int numThreads);
    ~ThreadPool();

    void enqueue(SOCKET clientSocket, const std::string& ip);
};