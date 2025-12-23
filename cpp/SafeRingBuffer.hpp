#pragma once 

#include <vector>
#include <atomic>
#include <optional>
#include <iostream>
#include <thread>

template<typename T>
class SafeRingBuffer {
    public:
    SafeRingBuffer(size_t capacity) : buffer_(capacity) { }

    bool push(const T& item) {
        const auto pos = back_.load();
        const auto next_pos = (pos + 1) % buffer_.size();
        if(next_pos == front_.load()) {
            return false;
        }
        buffer_[next_pos] = item;
        back_.store(next_pos);
        return true;
    }

    bool pop(T& item) {
        const auto pos = front_.load();
        if(pos == back_.load()) {
            return false;
        }
        item = buffer_[pos];
        const auto next_pos = (pos + 1) % buffer_.size();
        front_.store(next_pos);
        return true;
    }

    private:
    std::vector<T> buffer_;
    std::atomic<size_t> front_{0};
    std::atomic<size_t> back_{0};
};
