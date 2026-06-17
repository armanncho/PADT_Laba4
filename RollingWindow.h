#ifndef ROLLING_WINDOW_H
#define ROLLING_WINDOW_H

#include "Laba2/DynamicArray.h"
#include <stdexcept>

template<class T>
class RollingWindow {
private:
    DynamicArray<T> data_storage;

    int max_size;
    int current_size;

    int real_start;
    int logical_start;
    int logical_end;

public:
    explicit RollingWindow(int limit = 100);

    bool empty() const;
    void reset();

    int size() const;
    int limit() const;
    int start_index() const;
    int end_index() const;

    bool has_index(int virtual_idx) const;

    const T& fetch(int virtual_idx) const;
    void add(const T& element, int virtual_idx);
};

// --- Реализация методов ---

template<class T>
RollingWindow<T>::RollingWindow(int limit)
        : data_storage(limit), max_size(limit), current_size(0),
          real_start(0), logical_start(0), logical_end(0)
{
    if (limit <= 0) {
        throw std::invalid_argument("Limit for RollingWindow must be strictly positive");
    }
}

template<class T>
bool RollingWindow<T>::empty() const { return current_size == 0; }

template<class T>
int RollingWindow<T>::size() const { return current_size; }

template<class T>
int RollingWindow<T>::limit() const { return max_size; }

template<class T>
int RollingWindow<T>::start_index() const {
    if (empty()) throw std::logic_error("Window is currently empty");
    return logical_start;
}

template<class T>
int RollingWindow<T>::end_index() const {
    if (empty()) throw std::logic_error("Window is currently empty");
    return logical_end;
}

template<class T>
bool RollingWindow<T>::has_index(int virtual_idx) const {
    if (empty()) return false;
    return (virtual_idx >= logical_start) && (virtual_idx <= logical_end);
}

template<class T>
const T& RollingWindow<T>::fetch(int virtual_idx) const {
    if (!has_index(virtual_idx)) {
        throw std::out_of_range("Requested index is out of window bounds");
    }
    int shift = virtual_idx - logical_start;
    int actual_position = (real_start + shift) % max_size;
    return data_storage.Get(actual_position);
}

template<class T>
void RollingWindow<T>::add(const T& element, int virtual_idx) {
    if (empty()) {
        data_storage.Set(element, 0);
        real_start = 0;
        logical_start = virtual_idx;
        logical_end = virtual_idx;
        current_size = 1;
    }
    else if (current_size < max_size) {
        int next_pos = (real_start + current_size) % max_size;
        data_storage.Set(element, next_pos);
        logical_end = virtual_idx;
        current_size++;
    }
    else {
        data_storage.Set(element, real_start);
        real_start = (real_start + 1) % max_size;
        logical_start = virtual_idx - max_size + 1;
        logical_end = virtual_idx;
    }
}

template<class T>
void RollingWindow<T>::reset() {
    real_start = 0;
    logical_start = 0;
    logical_end = 0;
    current_size = 0;
}

#endif //LABA4_ROLLINGWINDLOW_H