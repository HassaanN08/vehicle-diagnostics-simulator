#pragma once
#include <cstddef>
#include <cassert>
#include <new>

template<typename T, std::size_t N> 
class RingBuffer {

    static_assert(N > 0, "Template parameter N must be a positive integer!");

    alignas(T) unsigned char data[N * sizeof(T)];
    std::size_t head = 0;
    std::size_t tail = 0;
    bool isFull = false;

    const T* addressCalculation(const size_t& index) const {
        const T* byteAddress = reinterpret_cast<const T*>(data + (index * sizeof(T)));
        return byteAddress;
    }

    T* addressCalculation(const size_t& index) {
        T* byteAddress = reinterpret_cast<T*>(data + (index * sizeof(T)));
        return byteAddress;
    }

    public:
        RingBuffer() = default;

        void push(const T& var) {
            T* head_ptr = addressCalculation(head);

            if (isFull) {
                head_ptr->~T();
                tail = (tail + 1) % N;
            }

            new (head_ptr) T(var);

            head = (head + 1) % N;

            isFull = (head == tail);
        }

        void push(T&& var) {
            T* head_ptr = addressCalculation(head);

            if (isFull) {
                head_ptr->~T();
                tail = (tail + 1) % N;
            }

            new (head_ptr) T(static_cast<T&&>(var));

            head = (head + 1) % N;

            isFull = (head == tail);
        }

        const T& at(const std::size_t index) const {
            assert(index < size());
            const T* index_ptr = addressCalculation((index + tail) % N);
            return *index_ptr;
        }

        std::size_t size() const {
            if (empty()) return 0;

            if (isFull) return N;

            if (head > tail && !isFull) return head - tail;

            return N + (head - tail);
        }

        std::size_t capacity() const {
            return N;
        }

        bool empty() const {
            return (head == 0 && tail == 0 && !isFull);
        }

        ~RingBuffer() {
            for (std::size_t i = 0; i < size(); i++) {
                T* index_ptr = addressCalculation((i + tail) % N);
                index_ptr->~T();
            }
        }

        RingBuffer(const RingBuffer&) = delete;
        RingBuffer& operator=(const RingBuffer&) = delete;

        RingBuffer(RingBuffer&&) = delete;
        RingBuffer& operator=(RingBuffer&&) = delete;
};