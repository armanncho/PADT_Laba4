#ifndef LAB2_MUTABLEARRAYSEQUENCE_H
#define LAB2_MUTABLEARRAYSEQUENCE_H

#include "ArraySequence.h"

template<class T>
class MutableArraySequence: public ArraySequence<T> {

public:

    MutableArraySequence(const T *items, int count);
    MutableArraySequence();
    MutableArraySequence(const MutableArraySequence &other);

    Sequence<T>* Instance() override;
    Sequence<T>* CreateEmptySequence() const override;
};

template<class T>
MutableArraySequence<T>::MutableArraySequence()
        : ArraySequence<T>() {}

template<class T>
MutableArraySequence<T>::MutableArraySequence(const T *items, int count)
        : ArraySequence<T>(items, count) {}

template<class T>
MutableArraySequence<T>::MutableArraySequence(const MutableArraySequence<T> &other)
        : ArraySequence<T>(other) {}

template<class T>
Sequence<T>* MutableArraySequence<T>::Instance() {
    return this;
}

template<class T>
Sequence<T>* MutableArraySequence<T>::CreateEmptySequence() const {
    return new MutableArraySequence<T>();
}

#endif //LAB2_MUTABLEARRAYSEQUENCE_H
