from collections import deque
import threading 
from typing import TypeVar, Generic, Optional 

T = TypeVar("T")

class SafeQueue(Generic[T]):
    def __init__(self):
        self.q = deque()
        self.cv = threading.Condition()
        self.shutdown = False

    def push(self, item: T) -> None:
        with self.cv:
            if self.shutdown:
                raise RuntimeError("Queue is shutdown")
            
            self.q.append(item)
            self.cv.notify()

    def pop(self) -> T:
        with self.cv:
            while len(self.q) == 0 and not self.shutdown:
                self.cv.wait()
            
            if len(self.q) == 0 and self.shutdown:
                raise RuntimeError("Queue is shutdown")
            
            return self.q.popleft()
        
    def try_pop(self) -> Optional[T]:
        with self.cv:
            if len(self.q) == 0:
                return None 

            return self.q.popleft() 

    def pop_for(self, duration) -> Optional[T]:
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

    def size(self) -> int:
        return len(self.q) 

    def clear(self) -> None:
        with self.cv:
            self.q = deque()

    def close(self) -> None:
        with self.cv:
            self.shutdown = True 
            self.cv.notify_all()
