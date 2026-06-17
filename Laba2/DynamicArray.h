#ifndef LABA2_DYNAMICARRAY_H
#define LABA2_DYNAMICARRAY_H

#include <stdexcept>
#include "I_enumerator.h"

template <class T>
class DynamicArray {
private:
    T *data;
    int size;
    int capacity;

public:
    // Конструкторы и деструктор
    DynamicArray(const T *items, int count);
    DynamicArray(int size);
    DynamicArray(const DynamicArray<T>& dynamicArray);
    ~DynamicArray();

    // Геттеры
    const T& Get(int index) const;
    int GetSize() const;

    // Операции
    void Set(const T& value, int index);
    void Resize(int newSize);

    // --- ВЛОЖЕННЫЙ КЛАСС ИТЕРАТОРА ---
    class ArrayEnumerator : public IEnumerator<T> {
    private:
        T* current;
        T* end;
    public:
        // Так как класс вложенный, он видит приватные поля DynamicArray
        ArrayEnumerator(const DynamicArray<T>* arr) {
            this->current = arr->data;
            this->end = arr->data + arr->size;
        }

        bool has_more_elements() const override {
            return current != end;
        }

        const T& next() override {
            if (!has_more_elements())
                throw std::out_of_range("ArrayEnumerator::next");
            return *current++;
        }
    };
    // ---------------------------------
};

/*============ РЕАЛИЗАЦИЯ МЕТОДОВ МАССИВА ============*/

template<class T>
DynamicArray<T>::DynamicArray(const T *items, int count) {
    if (count < 0) throw std::invalid_argument("Size cannot be negative");
    this->size = count;
    this->capacity = (count > 0) ? count : 1;
    data = new T[capacity];
    for (int i = 0; i < size; i++) data[i] = items[i];
}

template<class T>
DynamicArray<T>::DynamicArray(int size) {
    if (size < 0) throw std::invalid_argument("Size cannot be negative");
    this->size = size;
    this->capacity = (size > 0) ? size : 1;
    data = new T[capacity];
}

template<class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& dynamicArray) {
    this->size = dynamicArray.size;
    this->capacity = dynamicArray.capacity;
    data = new T[capacity];
    for (int i = 0; i < size; i++) data[i] = dynamicArray.data[i];
}

template<class T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

template<class T>
const T& DynamicArray<T>::Get(int index) const {
    if (index < 0 || index >= size) throw std::out_of_range("Index out of range");
    return data[index];
}

template<class T>
int DynamicArray<T>::GetSize() const {
    return size;
}

template<class T>
void DynamicArray<T>::Set(const T& value, int index) {
    if (index < 0 || index >= size) throw std::out_of_range("Index out of range");
    data[index] = value;
}

template<class T>
void DynamicArray<T>::Resize(int newSize) {
    if (newSize < 0) throw std::invalid_argument("Size cannot be negative");
    if (newSize <= capacity) {
        size = newSize;
        return;
    }
    int newCapacity = (capacity == 0) ? 1 : capacity;
    while (newCapacity < newSize) newCapacity *= 2;
    T* newData = new T[newCapacity];
    for (int i = 0; i < size; i++) newData[i] = data[i];
    delete[] data;
    data = newData;
    capacity = newCapacity;
    size = newSize;
}

#endif //LABA2_DYNAMICARRAY_H