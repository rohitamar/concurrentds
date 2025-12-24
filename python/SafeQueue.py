from collections import deque
import threading 

class SafeQueue:
    def __init__(self):
        self.q = deque()
        self.cv = threading.Condition()
        self.shutdown = False

    def push(self, item):
        with self.cv:
            if self.shutdown:
                raise RuntimeError("Queue is shutdown")
            
            self.q.append(item)
            self.cv.notify()

    def pop(self):
        with self.cv:
            while len(self.q) == 0 and not self.shutdown:
                self.cv.wait()
            
            if len(self.q) == 0 and self.shutdown:
                raise RuntimeError("Queue is shutdown")
            
            return self.q.popleft()
        
    def try_pop(self):
        with self.cv:
            if len(self.q) == 0:
                return None 

            return self.q.popleft() 

    def pop_for(self, duration):
        with self.cv:
            success = self.cv.wait_for(
                lambda: len(self.q) > 0 or self.shutdown,
                timeout=duration
            )

            if not success:
                return None 

            if len(self.q) == 0 and self.shutdown:
                raise RuntimeError("Queue is shutdown")

            return self.q.popleft()
          
    def empty(self) -> bool:
        return len(self.q) == 0 

    def size(self):
        return len(self.q) 

    def clear(self):
        with self.cv:
            self.q = deque()

    def close(self):
        with self.cv:
            self.shutdown = True 
            self.cv.notify_all()
