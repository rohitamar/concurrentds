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
