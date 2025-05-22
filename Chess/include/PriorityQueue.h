#pragma once
#include <list>
#include <stdexcept>
#include <string>

class QueueEmptyException : public std::exception {
public:
    QueueEmptyException() : message("Attempt to poll from an empty queue") {}
    const char* what() const noexcept override { return message.c_str(); }
private:
    std::string message;
};

class QueueFullException : public std::exception {
public:
    QueueFullException(int max_size) : message("Queue reached maximum capacity of " + std::to_string(max_size)) {}
    const char* what() const noexcept override { return message.c_str(); }
private:
    std::string message;
};

// Template class for the comparator
template <typename T>
struct DefaultComparator {
    int operator()(const T& a, const T& b) const {
        return a - b;
    }
};

// Template priority queue class
template <typename T, typename Comparator = DefaultComparator<T>>
class PriorityQueue {
private:
    std::list<T> queue;
    Comparator comparator;
    size_t max_size; // For limited-size queue implementations

public:
    // Constructor with optional max size (default is unlimited)
    PriorityQueue(size_t max = 0) : max_size(max) {}

    // Push element into the right position with O(n) complexity
    void push(const T& item) {
        // Check if queue has reached its maximum capacity
        if (max_size > 0 && queue.size() >= max_size) {
            // Check if the new item has higher priority than the lowest priority item
            if (!queue.empty() && comparator(item, queue.back()) > 0) {
                queue.pop_back(); // Remove lowest priority item
            } else {
                throw QueueFullException(max_size);
            }
        }

        // Find the appropriate position to insert the item
        auto it = queue.begin();
        while (it != queue.end() && comparator(item, *it) <= 0) {
            ++it;
        }
        queue.insert(it, item);

        // Ensure we never exceed max_size
        if (max_size > 0 && queue.size() > max_size) {
            queue.pop_back();
        }
    }

    // Poll the highest priority element with O(1) complexity
    T poll() {
        if (queue.empty()) {
            throw QueueEmptyException();
        }
        T front = queue.front();
        queue.pop_front();
        return front;
    }

    // Peek at the highest priority element without removing
    const T& peek() const {
        if (queue.empty()) {
            throw QueueEmptyException();
        }
        return queue.front();
    }

    // Get all elements in priority order (useful for move recommendations)
    std::list<T> getAll() const {
        return queue;
    }

    // Check if queue is empty
    bool empty() const {
        return queue.empty();
    }

    // Get current size of the queue
    size_t size() const {
        return queue.size();
    }

    // Clear the queue
    void clear() {
        queue.clear();
    }
};