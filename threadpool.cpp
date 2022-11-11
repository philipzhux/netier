#include "threadpool.h"

ThreadPool::ThreadPool(unsigned int size):_stop(false) {
    for(unsigned int i=0; i<size;i++) {
        _threads.emplace_back(std::thread([this](){
            while(true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(_mu);
                    while(!_stop&&_jobs.empty()) _qchange.wait(lock);
                    if(_stop && _jobs.empty()) return;
                    task = _jobs.front();
                    _jobs.pop();
                }
                task();
            }
        }));
    }
    
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(_mu);
        _stop = true;
    }
    _qchange.notify_all();
    for(auto &t:_threads) {
        if(t.joinable()) t.join();
    }
}