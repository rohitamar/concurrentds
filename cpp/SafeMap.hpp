#pragma once

#include <mutex>
#include <optional>
#include <unordered_map>
#include <shared_mutex>

template<typename K, typename V>
class SafeMap {
    public:
    SafeMap() = default;

    std::optional<V> get(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = map_.find(key);
        if(it != map_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool contains(const K& key) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return map_.find(key) != map_.end();
    }

    void put(const K& key, const V& value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        map_[key] = value;
    }

    size_t size() const { 
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return map_.size();
    }

    private:
    std::unordered_map<K, V> map_;
    mutable std::shared_mutex mutex_;
};