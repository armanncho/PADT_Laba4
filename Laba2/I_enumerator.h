#ifndef LAB2_I_ENUMERATOR_H
#define LAB2_I_ENUMERATOR_H

// универсальна для всех
template<class T>
class IEnumerator {

public:
    virtual ~IEnumerator() = default;

    virtual bool has_more_elements() const = 0;

    virtual const T& next() = 0;

};

#endif //LAB2_I_ENUMERATOR_H
