/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "threadpool.hpp"

ThreadPool::ThreadPool(unsigned int size) : __stop(false)
{
    for (unsigned int i = 0; i < size; i++)
    {
        __threads.emplace_back(std::thread([this]()
                                          {
            while(true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(__mu);
                    while(!__stop&&__jobs.empty()) __qchange.wait(lock);
                    if(__stop && __jobs.empty()) return;
                    task = __jobs.front();
                    __jobs.pop();
                }
                task();
            } }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(__mu);
        __stop = true;
    }
    __qchange.notify_all();
    for (auto &t : __threads)
    {
        if (t.joinable())
            t.join();
    }
}