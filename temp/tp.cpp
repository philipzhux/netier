
#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <queue>
#include <future>

class ThreadPool {
    private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
    std::mutex mu;
    std::condition_variable qcond;
    public:
    explicit ThreadPool(int t = std::thread::hardware_concurrency()) {
        for(int i=0; i<t; i++) 
            threads.push_back(std::thread(
                [this](){
                    std::function<void()> task;
                    while(true) {
                        {
                            std::
                        }
                    }
                    
                }
            ));
    }
};