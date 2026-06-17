#ifndef LABA2_LINKEDLIST_H
#define LABA2_LINKEDLIST_H

#include <stdexcept>
#include "I_enumerator.h"

template <class T>
class LinkedList {
private:
    struct Node {
        T data;
        Node *next;
        Node(T value) : data(value), next(nullptr) {}
    };

    Node *head;
    Node *tail;
    int length;

public:
    class ListEnumerator : public IEnumerator<T> {
    private:
        const Node *current;
    public:
        ListEnumerator(const Node* headNode) : current(headNode) {}

        bool has_more_elements() const override {
            return current != nullptr;
        }

        const T& next() override {
            if (!has_more_elements())
                throw std::out_of_range("ListEnumerator::next");
            const T& value = current->data;
            current = current->next;
            return value;
        }
    };

public:
    LinkedList();
    LinkedList(const T *element, int count);
    LinkedList(const LinkedList<T>& list);
    ~LinkedList();

    const T& GetFirst() const;
    const T& GetLast() const;
    const T& Get(int index) const;
    int GetLength() const;
    Node* GetHead() const; // Тот самый метод для итератора

    void Append(const T& element);
    void Prepend(const T& element);
    void InsertAt(const T& element, int index);
    void RemoveAt(int index);

    LinkedList<T>* GetSubList(int startIndex, int endIndex);
    LinkedList<T>* Concat(const LinkedList<T>& list) const;
};

/*============ РЕАЛИЗАЦИЯ ============*/

template<class T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), length(0) {}

template<class T>
LinkedList<T>::LinkedList(const T *element, int count) : LinkedList() {
    if (count < 0) throw std::out_of_range("Count negative");
    for (int i = 0; i < count; i++) Append(element[i]);
}

template<class T>
LinkedList<T>::LinkedList(const LinkedList<T>& list) : LinkedList() {
    Node *curr = list.head;
    while (curr != nullptr) {
        Append(curr->data);
        curr = curr->next;
    }
}

template<class T>
LinkedList<T>::~LinkedList() {
    Node *curr = head;
    while (curr != nullptr) {
        Node *next = curr->next;
        delete curr;
        curr = next;
    }
}

template<class T>
const T& LinkedList<T>::GetFirst() const {
    if (length == 0) throw std::out_of_range("Empty");
    return head->data;
}

template<class T>
const T& LinkedList<T>::GetLast() const {
    if (length == 0) throw std::out_of_range("Empty");
    return tail->data;
}

template<class T>
const T& LinkedList<T>::Get(int index) const {
    if (index < 0 || index >= length) throw std::out_of_range("Range error");
    Node *curr = head;
    for (int i = 0; i < index; i++) curr = curr->next;
    return curr->data;
}

template<class T>
int LinkedList<T>::GetLength() const {
    return length;
}

template<class T>
typename LinkedList<T>::Node* LinkedList<T>::GetHead() const {
    return head;
}

template<class T>
void LinkedList<T>::Append(const T& element) {
    Node *newNode = new Node(element);
    if (length == 0) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    length++;
}

template<class T>
void LinkedList<T>::Prepend(const T& element) {
    Node *newNode = new Node(element);
    newNode->next = head;
    head = newNode;
    if (length == 0) tail = newNode;
    length++;
}

template<class T>
void LinkedList<T>::InsertAt(const T& element, int index) {
    if (index < 0 || index > length) throw std::out_of_range("Range error");
    if (index == 0) { Prepend(element); return; }
    if (index == length) { Append(element); return; }
    Node *prev = head;
    for (int i = 0; i < index - 1; i++) prev = prev->next;
    Node *newNode = new Node(element);
    newNode->next = prev->next;
    prev->next = newNode;
    length++;
}

template<class T>
void LinkedList<T>::RemoveAt(int index) {
    if (index < 0 || index >= length) throw std::out_of_range("Range error");
    if (index == 0) {
        Node *toDelete = head;
        head = head->next;
        delete toDelete;
        length--;
        if (length == 0) tail = nullptr;
        return;
    }
    Node *prev = head;
    for (int i = 0; i < index - 1; i++) prev = prev->next;
    Node *toDelete = prev->next;
    prev->next = toDelete->next;
    if (toDelete == tail) tail = prev;
    delete toDelete;
    length--;
}

template<class T>
LinkedList<T>* LinkedList<T>::Concat(const LinkedList<T>& list) const {
    LinkedList<T> *res = new LinkedList<T>(*this);
    Node *curr = list.head;
    while (curr != nullptr) {
        res->Append(curr->data);
        curr = curr->next;
    }
    return res;
}

template<class T>
LinkedList<T>* LinkedList<T>::GetSubList(int startIndex, int endIndex) {
    if (startIndex < 0 || startIndex >= length || startIndex > endIndex || endIndex >= length)
        throw std::out_of_range("Range error");
    LinkedList<T> *newList = new LinkedList<T>();
    Node *curr = head;
    for (int i = 0; i < startIndex; i++) curr = curr->next;
    for (int i = startIndex; i <= endIndex; i++) {
        newList->Append(curr->data);
        curr = curr->next;
    }
    return newList;
}

#endif