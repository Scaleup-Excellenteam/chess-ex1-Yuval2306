#pragma once
#include "PriorityQueue.h"
#include <mutex>
#include <vector>

template <typename T, typename Comparator = DefaultComparator<T>>
class ThreadSafePriorityQueue {
private:
    PriorityQueue<T, Comparator> queue;
    mutable std::mutex queueMutex;

public:
    // Constructor
    ThreadSafePriorityQueue(size_t max = 0) : queue(max) {}

    // Thread-safe push
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(queueMutex);
        queue.push(item);
    }

    // Thread-safe poll
    T poll() {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queue.poll();
    }

    // Thread-safe peek
    T peek() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queue.peek();
    }

    // Thread-safe empty check
    bool empty() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queue.empty();
    }

    // Thread-safe size
    size_t size() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queue.size();
    }

    // Thread-safe clear
    void clear() {
        std::lock_guard<std::mutex> lock(queueMutex);
        queue.clear();
    }

    // Get all elements (creates a copy for thread safety)
    std::vector<T> getAllCopy() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        std::list<T> items = queue.getAll();
        return std::vector<T>(items.begin(), items.end());
    }
};