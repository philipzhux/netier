
#include <queue>
#include <vector>
class Buffer
{
private:
    std::vector<char> __buffer;

public:
    Buffer();
    ~Buffer();
    void put(const void *buf, size_t size);
    void *getWriter(size_t size);
    const void *getReader();
    void moveReadCursor(size_t size);
    std::vector<char> get(size_t size);
    std::vector<char> get();
    const std::vector<char> &getInnerVec();
    void shrink(size_t size);
    void clear();
    size_t size();
};