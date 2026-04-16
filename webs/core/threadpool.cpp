#include "threadpool.h"
#include <iostream>

extern void handleClient(SOCKET clientSocket);

ThreadPool::ThreadPool(int numThreads) : stop(false) {

    for (int i = 0; i < numThreads; i++) {
        workers.emplace_back(&ThreadPool::worker, this);
    }
}

void ThreadPool::worker() {

    while (true) {

        SOCKET clientSocket;

        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [this]() {
                return stop || !tasks.empty();
            });

            if (stop && tasks.empty()) return;

            clientSocket = tasks.front();
            tasks.pop();
        }

        handleClient(clientSocket);
    }
}

void ThreadPool::enqueue(SOCKET clientSocket) {

    {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(clientSocket);
    }

    cv.notify_one();
}

ThreadPool::~ThreadPool() {

    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }

    cv.notify_all();

    for (std::thread& t : workers) {
        t.join();
    }
}