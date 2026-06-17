#ifndef LABA4_UTILS_H
#define LABA4_UTILS_H

template<class A, class B>
struct Duo { // data holder
    A left;
    B right;

    Duo()
            : left(), right() {}

    Duo(const A& left,const B& right)
            : left(left), right(right) {}
};

#endif //LABA4_UTILS_H
