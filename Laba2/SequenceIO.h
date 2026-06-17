#ifndef LAB2_SEQUENCEIO_H
#define LAB2_SEQUENCEIO_H

#include "Sequence.h"
#include <ostream>

template<class T>
std::ostream& operator<<(std::ostream &os, const Sequence<T> &seq) {
    auto it = seq.GetEnumerator();
    os << "[";
    while (it->has_more_elements()) {
        os << it->next();
        if (it->has_more_elements())
            os << ", ";
    }
    os << "]";
    delete it;
    return os;
}

#endif //LAB2_SEQUENCEIO_H
