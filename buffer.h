
#include <queue>
#include <mutex>
class Buffer{
    private:
    std::queue<unsigned char> _wbuffer;
    // std::queue<unsigned char> _rbuffer;
    // std::mutex _mu;
    // mu not needed, a client will only be served with one thread
    public:
    Buffer();
    ~Buffer();
    void asyncWrite(void* buffer, size_t size);
    // size_t read(void* buffer, size_t size);
    // void put(void* buffer, size_t size);
    size_t get(void* buffer, size_t size);
};