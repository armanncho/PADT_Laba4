#ifndef LABA2_ARRAYSEQUENCE_H
#define LABA2_ARRAYSEQUENCE_H

#include "Sequence.h"
#include "DynamicArray.h"
#include <stdexcept>


template <class T>
class ArraySequence : public Sequence<T> {
private:
    DynamicArray<T>* items;

public:
    ArraySequence();
    ArraySequence(const T* items_arr, int count);
    ArraySequence(const ArraySequence<T>& other);

    ~ArraySequence() override;

    Sequence<T>* Instance() override;
    Sequence<T>* CreateEmptySequence() const override;

    IEnumerator<T>* GetEnumerator() const override;

    const T& GetFirst() const override;
    const T& GetLast() const override;
    const T& Get(int index) const override;
    int GetLength() const override;

    void AppendInternal(const T& item) override;
    void PrependInternal(const T& item) override;
    void InsertAtInternal(const T& item, int index) override;
    void RemoveAtInternal(int index) override;
};

/*============ РЕАЛИЗАЦИЯ МЕТОДОВ ============*/

template <class T>
ArraySequence<T>::ArraySequence() {
    items = new DynamicArray<T>(0);
}

template <class T>
ArraySequence<T>::ArraySequence(const T* items_arr, int count) {
    items = new DynamicArray<T>(items_arr, count);
}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& other) {
    items = new DynamicArray<T>(*(other.items));
}

template <class T>
ArraySequence<T>::~ArraySequence() {
    delete items;
}

template <class T>
Sequence<T>* ArraySequence<T>::Instance() {
    return this;
}

template <class T>
Sequence<T>* ArraySequence<T>::CreateEmptySequence() const {
    return new ArraySequence<T>();
}

template<class T>
IEnumerator<T>* ArraySequence<T>::GetEnumerator() const {
    return new typename DynamicArray<T>::ArrayEnumerator(this->items);
}

template <class T>
const T& ArraySequence<T>::GetFirst() const {
    if (items->GetSize() == 0) throw std::out_of_range("Sequence is empty");
    return items->Get(0);
}

template <class T>
const T& ArraySequence<T>::GetLast() const {
    int length = items->GetSize();
    if (length == 0) throw std::out_of_range("Sequence is empty");
    return items->Get(length - 1);
}

template <class T>
const T& ArraySequence<T>::Get(int index) const {
    return items->Get(index);
}

template <class T>
int ArraySequence<T>::GetLength() const {
    return items->GetSize();
}

template <class T>
void ArraySequence<T>::AppendInternal(const T& item) {
    int oldSize = items->GetSize();
    items->Resize(oldSize + 1);
    items->Set(item, oldSize);
}

template <class T>
void ArraySequence<T>::PrependInternal(const T& item) {
    int oldSize = items->GetSize();
    items->Resize(oldSize + 1);
    for (int i = oldSize; i > 0; --i) {
        items->Set(items->Get(i - 1), i);
    }
    items->Set(item, 0);
}

template <class T>
void ArraySequence<T>::InsertAtInternal(const T& item, int index) {
    int oldSize = items->GetSize();
    if (index < 0 || index > oldSize) throw std::out_of_range("Index out of range");
    items->Resize(oldSize + 1);
    for (int i = oldSize; i > index; --i) {
        items->Set(items->Get(i - 1), i);
    }
    items->Set(item, index);
}

template <class T>
void ArraySequence<T>::RemoveAtInternal(int index) {
    int oldSize = items->GetSize();
    if (index < 0 || index >= oldSize) throw std::out_of_range("Index out of range");
    for (int i = index; i < oldSize - 1; ++i) {
        items->Set(items->Get(i + 1), i);
    }
    items->Resize(oldSize - 1);
}

#endif // LABA2_ARRAYSEQUENCE_H