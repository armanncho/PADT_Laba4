#ifndef LAB2_IMMUTABLELISTSEQUENCE_H
#define LAB2_IMMUTABLELISTSEQUENCE_H


#include "ListSequence.h"

template<class T>
class ImmutableListSequence : public  ListSequence<T>{

public:
    ImmutableListSequence(const T *items, int count);
    ImmutableListSequence();
    ImmutableListSequence(const ImmutableListSequence &other);

    Sequence<T>* Instance() override;
    Sequence<T>* CreateEmptySequence() const override;

};

template<class T>
ImmutableListSequence<T>::ImmutableListSequence(const T *items, int count)
        : ListSequence<T>(items, count) {}

template<class T>
ImmutableListSequence<T>::ImmutableListSequence()
        : ListSequence<T>() {}

template<class T>
ImmutableListSequence<T>::ImmutableListSequence(const ImmutableListSequence &other)
        : ListSequence<T>(other) {}

template<class T>
Sequence<T>* ImmutableListSequence<T>::Instance() {
    return new ImmutableListSequence<T>(*this);
}

template<class T>
Sequence<T> *ImmutableListSequence<T>::CreateEmptySequence() const {
    return new ImmutableListSequence<T>();
}


#endif //LAB2_IMMUTABLELISTSEQUENCE_H
