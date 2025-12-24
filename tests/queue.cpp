#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "../cpp/SafeQueue.hpp"

std::mutex mtx;

const int num_threads = 5;
const int gen_per_thread = 100;

void log(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << message << std::endl;
}

void producer(SafeQueue<int>& q, int id) {
    for(int i = 0; i < gen_per_thread; i++) {
        q.push(i + id * gen_per_thread * 10);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    log("Producer " + std::to_string(id) + " finished pushing.");
}

void consumer(SafeQueue<int>& q, int id) {
    while(true) {
        try {
            int item = q.pop();
        } catch (const std::runtime_error& e) {
            log("Consumer " + std::to_string(id) + " terminated.");
            break;
        }
    }
}

int main() {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    SafeQueue<int> queue;
    for(int i = 0; i < num_threads; i++) {
        consumers.emplace_back(consumer, std::ref(queue), i);
    }

    for(int i = 0; i < num_threads; i++) {
        producers.emplace_back(producer, std::ref(queue), i);
    }

    // produce data
    for(auto &th : producers) {
        if(th.joinable()) th.join();
    }

    // poison pill (indicates producer is done making new instances)
    queue.close();

    // wait for consumers to finish processing
    for(auto &th : consumers) {
        if(th.joinable()) th.join();
    }
}