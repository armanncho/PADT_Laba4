#ifndef LABA2_LISTSEQUENCE_H
#define LABA2_LISTSEQUENCE_H

#include "Sequence.h"
#include "LinkedList.h"
#include <stdexcept>


template<class T>
class ListSequence : public Sequence<T> {
private:
    LinkedList<T>* items;

public:
    ListSequence();
    ListSequence(T* items_arr, int count);
    ListSequence(const ListSequence<T>& other);
    ~ListSequence() override;

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
ListSequence<T>::ListSequence() {
    items = new LinkedList<T>();
}

template <class T>
ListSequence<T>::ListSequence(T* items_arr, int count) {
    items = new LinkedList<T>(items_arr, count);
}

template <class T>
ListSequence<T>::ListSequence(const ListSequence<T>& other) {
    items = new LinkedList<T>(*(other.items));
}

template <class T>
ListSequence<T>::~ListSequence() {
    delete items;
}

template <class T>
Sequence<T>* ListSequence<T>::Instance() {
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::CreateEmptySequence() const {
    return new ListSequence<T>();
}

template<class T>
IEnumerator<T> *ListSequence<T>::GetEnumerator() const {
    return new typename LinkedList<T>::ListEnumerator(this->items->GetHead());
}

template <class T>
const T& ListSequence<T>::GetFirst() const {
    return items->GetFirst();
}

template <class T>
const T& ListSequence<T>::GetLast() const {
    return items->GetLast();
}

template <class T>
const T& ListSequence<T>::Get(int index) const {
    return items->Get(index);
}

template <class T>
int ListSequence<T>::GetLength() const {
    return items->GetLength();
}

template <class T>
void ListSequence<T>::AppendInternal(const T& item) {
    items->Append(item);
}

template <class T>
void ListSequence<T>::PrependInternal(const T& item) {
    items->Prepend(item);
}

template <class T>
void ListSequence<T>::InsertAtInternal(const T& item, int index) {
    items->InsertAt(item, index);
}

template <class T>
void ListSequence<T>::RemoveAtInternal(int index) {
    items->RemoveAt(index);
}

#endif // LABA2_LISTSEQUENCE_H