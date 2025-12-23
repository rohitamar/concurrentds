#ifndef SAFE_QUEUE_HPP
#define SAFE_QUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <optional>
#include <queue>
#include <mutex>

template<typename T>
class SafeQueue {
    public:
    SafeQueue() = default;

    // blocking 
    void push(T item);
    T pop();

    // non-blocking
    std::optional<T> try_pop();

    // timeout blocking
    std::optional<T> pop_for(std::chrono::milliseconds duration);

    // utility
    void empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::queue<T> empty;
        std::swap(queue_, empty);
    }

    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
        cv_.notify_all();
    }

    private:
    mutable std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool shutdown_ = false;
};

template<typename T>
void SafeQueue<T>::push(T item) {
    std::lock_guard<std::mutex> lock(mutex_);
    if(shutdown_) {
        throw std::runtime_error("Queue is shutdown");
    }
    queue_.push(item);
    cv_.notify_one();
}

template<typename T>
T SafeQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [] { 
        return !queue_.empty() || shutdown_; 
    });

    if(queue_.empty() && shutdown_) {
        throw std::runtime_error("Queue is shutdown");
    }

    T item = std::move(queue_.front());
    queue_.pop();
    return item;
}

template<typename T>
std::optional<T> SafeQueue<T>::try_pop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if(queue_.empty()) {
        return std::nullopt;
    }

    T item = std::move(queue_.front());
    queue_.pop();
    return item;
}

template<typename T>
std::optional<T> SafeQueue<T>::pop_for(std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(mutex_);

    if(!cv_.wait_for(lock, duration, [this] { return !queue_.empty() || shutdown_; })) {
        return std::nullopt;
    }

    if(queue_.empty()) {
        return std::nullopt;
    }

    T item = std::move(queue_.front());
    queue_.pop();
    return item;
}

#endif