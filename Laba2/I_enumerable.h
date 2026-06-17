#ifndef LAB2_I_ENUMERABLE_H
#define LAB2_I_ENUMERABLE_H

#include "I_enumerator.h"

template<class T>
class IEnumerable {

public:
    virtual ~IEnumerable() = default;

    virtual IEnumerator<T>* GetEnumerator() const = 0;

};

#endif //LAB2_I_ENUMERABLE_H
