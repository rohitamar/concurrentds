#include <iostream>
#include <thread>
#include <vector>

#include "../cpp/SafeMap.hpp"

const int num_threads = 20;
const int size_per_thread = 1000000;

void writer(SafeMap<int, int>& m, int start, int end) {
    for(int i = start; i <= end; i++) {
        m.put(i, i);
    }
}

void reader(SafeMap<int, int>& m, int start, int end) {
    for(int i = start; i <= end; i++) {
        m.contains(i);
    }
}

int main() {
    SafeMap<int, int> safeMap;
    std::vector<std::thread> threads;
    for(int i = 0; i < num_threads; i++) {
        threads.emplace_back(
            writer, 
            std::ref(safeMap),
            i * size_per_thread,
            (i + 1) * size_per_thread - 1
        );
    }

    for(int i = 0; i < num_threads; i++) {
        threads.emplace_back(
            reader, 
            std::ref(safeMap),
            i * size_per_thread,
            (i + 1) * size_per_thread - 1
        );
    }

    for(auto &th : threads) {
        if(th.joinable()) th.join();
    }

    std::cout << "Size: " << safeMap.size() << "\n";
}