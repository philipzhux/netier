#ifndef THREAD_H
#define THREAD_H
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
    // a context in golang with fx return at write side
    template <class T, class... A>
    auto syncRunJob(T&& fx, A&& ...args) -> std::future<typename std::result_of<T()>::type> {
        using return_type = typename std::result_of<T()>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<T>(fx),std::forward<A>(args)...));
        auto ret = task->get_future();
        {
            std::unique_lock<std::mutex> lock(_mu);
            _jobs.emplace([task](){(*task)();});
            if(_stop) throw std::runtime_error("Trying to add task to a stopped thread pool.");
        }
        _qchange.notify_one();
        return ret;
    }

private:
    std::vector<std::thread> _threads;
    std::queue<std::function<void()>> _jobs;
    std::mutex _mu;
    std::condition_variable _qchange;
    bool _stop;
};




#endif