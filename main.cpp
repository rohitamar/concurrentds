#include <thread>
#include <vector>

#include "cpp/SafeQueue.hpp"

void worker(SafeQueue<int>& q) {
    for(int i = 0; i < 10; i++) {
        q.push(i);
    }
}

int main() {
    SafeQueue<int> q;
    std::vector<std::thread> threads;
    for(int i = 0; i < 2; i++) {
        threads.emplace_back(std::ref(worker));
    }

    for(auto &th : threads) {
        th.join();
    }
}