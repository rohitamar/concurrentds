# concurrentds
Concurrent data structures

C++:
```c++
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
```

Python:
```python
import threading 
from python.SafeQueue import SafeQueue

def worker(q):
    for i in range(10):
        q.push(i)

if __name__ == '__main__':
    q = SafeQueue()
    threads = []
    for i in range(2):
        t = threading.Thread(
            target=worker,
            args=(q,)
        )
        t.start()
        threads.append(t)
    
    for t in threads:
        t.join()
```
