#ifndef LAB2_IMMUTABLEARRAYSEQUENCE_H
#define LAB2_IMMUTABLEARRAYSEQUENCE_H

#include "ArraySequence.h"

template <class T>
class ImmutableArraySequence : public ArraySequence<T> {

public:

    ImmutableArraySequence(const T *items, int count);
    ImmutableArraySequence();
    ImmutableArraySequence(const ImmutableArraySequence<T> &other);

    Sequence<T>* Instance() override;
    Sequence<T>* CreateEmptySequence() const override;
};

template<class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const T *items, int count)
        : ArraySequence<T>(items, count){}

template<class T>
ImmutableArraySequence<T>::ImmutableArraySequence()
        : ArraySequence<T>() {}

template<class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ImmutableArraySequence<T> &other)
        : ArraySequence<T>(other) {}

template<class T>
Sequence<T>* ImmutableArraySequence<T>::Instance() {
    return new ImmutableArraySequence<T>(*this);
}

template<class T>
Sequence<T>* ImmutableArraySequence<T>::CreateEmptySequence() const {
    return new ImmutableArraySequence<T>();
}

#endif //LAB2_IMMUTABLEARRAYSEQUENCE_H
