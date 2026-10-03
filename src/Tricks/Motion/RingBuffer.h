// My implementation of a ring buffer
#pragma once
#include <iostream>

template<typename T>
class RingBuffer {
public:
    RingBuffer(T , int size);
    put(T);
private:
    std::vector<T> buffer;
};
