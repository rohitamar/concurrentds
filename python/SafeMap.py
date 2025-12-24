import threading
from typing import TypeVar, Generic, Optional

K = TypeVar('K')
V = TypeVar('V')

class SafeMap(Generic[K, V]):
    def __init__(self):
        self.map = {}
        self.lock = threading.Lock() 

    def get(self, key: K) -> Optional[V]:
        with self.lock:
            return self.map.get(key) 

    def contains(self, key: K) -> bool:
        with self.lock:
            return key in self.map

    def put(self, key: K, value: V):
        with self.lock:
            self.map[key] = value

    def size(self) -> int:
        with self.lock:
            return len(self.map)