#ifndef LABA2_SEQUENCE_H
#define LABA2_SEQUENCE_H

#include "I_enumerable.h"
#include "Option.h"
#include <stdexcept>

template <class T>
class Sequence : public IEnumerable<T> {
public:
    // Деструктор
    virtual ~Sequence() = default;

    virtual IEnumerator<T>* GetEnumerator() const override = 0;
    virtual Sequence<T>* Instance() = 0;
    virtual Sequence<T>* CreateEmptySequence() const = 0;

    // Геттеры (реализуются в ArraySequence и ListSequence)
    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;
    virtual const T& Get(int index) const = 0;
    virtual int GetLength() const = 0;

    // Options (Безопасные геттеры)
    Option<T> TryGetFirst() const;
    Option<T> TryGetLast() const;
    Option<T> TryGet(int index) const;

    // Основные операции (общая логика)
    Sequence<T>* Append(const T& item);
    Sequence<T>* Prepend(const T& item);
    Sequence<T>* InsertAt(const T& item, int index);
    Sequence<T>* RemoveAt(int index);

    Sequence<T>* GetSubSequence(int startIndex, int endIndex) const;
    Sequence<T>* Concat(const Sequence<T> &list) const;

    // Функциональные методы (через итераторы)
    Sequence<T>* Map(T (*func)(const T&)) const;
    Sequence<T>* Where(bool (*pred)(const T&)) const;
    T Reduce(T (*func)(const T&, const T&), const T& init_value) const;

    // Перегрузка операторов
    const T& operator[](int index) const;
    bool operator==(const Sequence<T> &other) const;
    Sequence<T>* operator+(const Sequence<T> &other) const;

protected:
    // Внутренние методы реализации (реализуются в ArraySequence и ListSequence)
    virtual void AppendInternal(const T& item) = 0;
    virtual void PrependInternal(const T& item) = 0;
    virtual void InsertAtInternal(const T& item, int index) = 0;
    virtual void RemoveAtInternal(int index) = 0;
};

/*============ РЕАЛИЗАЦИЯ МЕТОДОВ ============*/

template<class T>
Sequence<T>* Sequence<T>::Append(const T &item) {
    Sequence<T> *inst = Instance();
    inst->AppendInternal(item);
    return inst;
}

template<class T>
Sequence<T>* Sequence<T>::Prepend(const T& item) {
    Sequence<T> *inst = Instance();
    inst->PrependInternal(item);
    return inst;
}

template<class T>
Sequence<T>* Sequence<T>::InsertAt(const T& item, int index) {
    Sequence<T> *inst = Instance();
    inst->InsertAtInternal(item, index);
    return inst;
}

template<class T>
Sequence<T>* Sequence<T>::RemoveAt(int index) {
    if (index < 0 || index >= GetLength())
        throw std::out_of_range("Index out of range");
    Sequence<T>* inst = Instance();
    inst->RemoveAtInternal(index);
    return inst;
}

template<class T>
Sequence<T>* Sequence<T>::Concat(const Sequence<T> &list) const{
    Sequence<T> *res = this->CreateEmptySequence();
    auto it1 = this->GetEnumerator();
    while (it1->has_more_elements())
        res->AppendInternal(it1->next());
    delete it1;

    auto it2 = list.GetEnumerator();
    while (it2->has_more_elements())
        res->AppendInternal(it2->next());
    delete it2;

    return res;
}

template<class T>
Sequence<T>* Sequence<T>::GetSubSequence(int startIndex, int endIndex) const{
    int length = this->GetLength();
    if (startIndex < 0 || startIndex >= length || endIndex < 0 || endIndex >= length || startIndex > endIndex)
        throw std::out_of_range("Index out of range");

    Sequence<T> *subSequence = this->CreateEmptySequence();
    auto it = this->GetEnumerator();
    int index = 0;

    while (it->has_more_elements()) {
        const T& value = it->next();
        if (index >= startIndex && index <= endIndex)
            subSequence->AppendInternal(value);
        index++;
    }
    delete it;
    return subSequence;
}

template<class T>
Sequence<T>* Sequence<T>::Map(T (*func)(const T&)) const{
    Sequence<T> *result = this->CreateEmptySequence();
    auto it = this->GetEnumerator();
    while (it->has_more_elements())
        result->AppendInternal(func(it->next()));
    delete it;
    return result;
}

template<class T>
Sequence<T>* Sequence<T>::Where(bool (*pred)(const T&)) const{
    Sequence<T> *result = this->CreateEmptySequence();
    auto it = this->GetEnumerator();
    while (it->has_more_elements()) {
        const T& value = it->next();
        if (pred(value))
            result->AppendInternal(value);
    }
    delete it;
    return result;
}

template<class T>
T Sequence<T>::Reduce(T (*func)(const T&, const T&), const T& init_value) const{
    T value = init_value;
    auto it = this->GetEnumerator();
    while (it->has_more_elements())
        value = func(value, it->next());
    delete it;
    return value;
}

template<class T>
const T& Sequence<T>::operator[](int index) const {
    return this->Get(index);
}

template<class T>
bool Sequence<T>::operator==(const Sequence<T> &other) const {
    if (this->GetLength() != other.GetLength())
        return false;
    if (this == &other)
        return true;

    auto it1 = this->GetEnumerator();
    auto it2 = other.GetEnumerator();
    while (it1->has_more_elements() && it2->has_more_elements()) {
        if (it1->next() != it2->next()) {
            delete it1; delete it2;
            return false;
        }
    }
    delete it1; delete it2;
    return true;
}

template<class T>
Sequence<T> *Sequence<T>::operator+(const Sequence<T> &other) const {
    return this->Concat(other);
}

/*============ OPTION FUNCTIONS ============*/

template<class T>
Option<T> Sequence<T>::TryGetFirst() const {
    if (this->GetLength() == 0) return Option<T>();
    return Option<T>(this->GetFirst());
}

template<class T>
Option<T> Sequence<T>::TryGetLast() const {
    if (this->GetLength() == 0) return Option<T>();
    return Option<T>(this->GetLast());
}

template<class T>
Option<T> Sequence<T>::TryGet(int index) const {
    if (index >= this->GetLength() || index < 0)
        return Option<T>();
    return Option<T>(this->Get(index));
}

#endif //LABA2_SEQUENCE_H