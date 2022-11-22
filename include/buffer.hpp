
/*
 * Modified on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include <queue>
#include <vector>

namespace netier {

class Buffer
{
private:
    std::vector<char> __buffer;

public:
    Buffer();
    Buffer(Buffer &&);
    Buffer(const Buffer &) = delete;
    ~Buffer();
    Buffer &operator=(Buffer &&other);
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
} // namespace netier
