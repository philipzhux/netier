#include "buffer.h"
#include <assert.h>

void Buffer::asyncWrite(void* buffer, size_t size){
    for(size_t i = 0;i<size;i++) _wbuffer.push(*static_cast<unsigned char*>(buffer+i));
}


size_t Buffer::get(void* buffer, size_t size){
    size_t cnt = 0;
    for(size_t i = 0;!_wbuffer.empty() && i<size;i++) {
        *static_cast<unsigned char*>(buffer+i) = _wbuffer.front();
        _wbuffer.pop();
        cnt += 1;
    }
    return cnt;
}


// void Buffer::put(void* buffer, size_t size){


// }
// void Buffer::get(void* buffer, size_t size){
    
// }
