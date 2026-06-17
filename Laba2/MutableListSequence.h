#ifndef LAB2_MUTABLELISTSEQUENCE_H

#include "ListSequence.h"

template<class T>
class MutableListSequence : public ListSequence<T> {

public:

    MutableListSequence(const T *items, int count);
    MutableListSequence();
    MutableListSequence(const MutableListSequence &other);

    Sequence<T>* Instance() override;
    Sequence<T>* CreateEmptySequence() const override;
};

template<class T>
MutableListSequence<T>::MutableListSequence(const T *items, int count)
        : ListSequence<T>(items, count) {}

template<class T>
MutableListSequence<T>::MutableListSequence()
        : ListSequence<T>() {}

template<class T>
MutableListSequence<T>::MutableListSequence(const MutableListSequence &other)
        : ListSequence<T>(other) {}

template<class T>
Sequence<T>* MutableListSequence<T>::Instance() {
    return this;
}

template<class T>
Sequence<T>* MutableListSequence<T>::CreateEmptySequence() const{
    return new MutableListSequence<T>();
}


#endif //LAB2_MUTABLELISTSEQUENCE_H
