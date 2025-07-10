#pragma once

#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include <stdexcept>

template<typename T>
class Disruptor {
public:
    explicit Disruptor(size_t size)
        : buffer_(size), size_(size), index_mask_(size - 1),
          producer_sequence_(0)
    {
        if ((size & (size - 1)) != 0) {
            throw std::runtime_error("Disruptor size must be power of 2");
        }
    }

    // Publish new event (by producer)
    void publish(const T& event) {
        uint64_t seq = producer_sequence_.fetch_add(1, std::memory_order_relaxed);
        buffer_[seq & index_mask_] = event;
    }

    // Create a new consumer cursor initialized to current producer sequence
    uint64_t create_consumer() const {
        return producer_sequence_.load(std::memory_order_relaxed);
    }

    // Consumer reads next available event based on its own cursor
    // Returns true if new data was read, false if no new data yet
    bool consume(uint64_t& consumer_sequence, T& out_event) const {
        uint64_t current_prod = producer_sequence_.load(std::memory_order_acquire);

        if (consumer_sequence < current_prod) {
            uint64_t next_seq = consumer_sequence;
            out_event = buffer_[next_seq & index_mask_];
            consumer_sequence++;
            return true;
        }
        return false;  // nothing new
    }

    // Returns number of unconsumed events
    size_t lag(uint64_t consumer_sequence) const {
        uint64_t current_prod = producer_sequence_.load(std::memory_order_relaxed);
        return static_cast<size_t>(current_prod - consumer_sequence);
    }

private:
    std::vector<T> buffer_;
    size_t size_;
    size_t index_mask_;
    std::atomic<uint64_t> producer_sequence_;
};

//#endif
