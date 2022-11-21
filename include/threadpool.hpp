/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#pragma once
#include <thread>
#include <functional>
#include <future>
#include <type_traits>
#include <vector>
#include <queue>
#include <stdexcept>
class ThreadPool
{

public:
    explicit ThreadPool(unsigned int size = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    // the future decouple the return value
    // with the funciton invocation, acting like
    // a channel in golang with fx return at write side
    template <class T, class... A>
    auto asyncRunJob(T &&fx, A &&...args) -> std::future<typename std::result_of<T()>::type>
    {
        using return_type = typename std::result_of<T()>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<T>(fx), std::forward<A>(args)...));
        auto ret = task->get_future();
        {
            std::unique_lock<std::mutex> lock(__mu);
            __jobs.emplace([task]()
                          { (*task)(); });
            if (__stop)
                throw std::runtime_error("Trying to add task to a stopped thread pool.");
        }
        __qchange.notify_one();
        return ret;
    };

    inline unsigned int getSize() const {
        return __threads.size();
    };

private:
    std::vector<std::thread> __threads;
    std::queue<std::function<void()>> __jobs;
    std::mutex __mu;
    std::condition_variable __qchange;
    bool __stop;
};
