import sys 
import os 

cur_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.dirname(cur_dir)
sys.path.insert(0, project_root)

from python.SafeQueue import SafeQueue
import time
import threading 

num_threads = 5
gen_per_thread = 100
log_lock = threading.Lock() 

def log(message):
    with log_lock:
        print(message)

def producer(q, id):
    for i in range(gen_per_thread):
        val = i + (id * gen_per_thread * 10)
        q.push(val)
        
        time.sleep(0.01)
        
    log(f"Producer {id} finished pushing.")

def consumer(q, id):
    while True:
        try:
            item = q.pop()
            log(f"Consumer {id} received {item}.")
        except RuntimeError:
            log(f"Consumer {id} terminated.")
            break

if __name__ == "__main__":
    producers = []
    consumers = []
    queue = SafeQueue()

    for i in range(num_threads):
        t = threading.Thread(target=consumer, args=(queue, i))
        t.start()
        consumers.append(t)

    for i in range(num_threads):
        t = threading.Thread(target=producer, args=(queue, i))
        t.start()
        producers.append(t)

    for t in producers:
        t.join()

    queue.close()

    for t in consumers:
        t.join()
        
    print("All threads finished.")