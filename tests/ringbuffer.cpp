#include <chrono>
#include <thread> 
#include "../cpp/SafeRingBuffer.hpp"

int main() {
    SafeRingBuffer<int> queue(10);
    std::atomic<bool> production_complete{false};
    std::thread producer([&]() {
        for(int i = 0; i < 50; i++) {
            while(!queue.push(i)) {
                std::this_thread::yield();
            }
            std::cout << "Producer pushed " << i << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        production_complete = true;
    });

    std::thread consumer([&]() {
        int value;
        while(true) {
            if(queue.pop(value)) {
                std::cout << "Consumer received " << value << "\n";
            } else {
                if(production_complete) {
                    break;
                }
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();
}