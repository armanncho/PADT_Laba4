#ifndef LABA4_LAZY_ALGEBRA_H
#define LABA4_LAZY_ALGEBRA_H

#include <stdexcept>

#include "Ordinal.h"
#include "LazySequence.h"
#include "Utils.h"

// 1. АЛГЕБРА ОРДИНАЛЬНЫХ ЧИСЕЛ

class OrdinalAlgebra {
public:
    static Ordinal Add(const Ordinal& left, const Ordinal& right) {
        return left + right;
    }

    static Ordinal Subtract(const Ordinal& left, const Ordinal& right) {
        return left - right;
    }

    static Ordinal Infimum(const Ordinal& a, const Ordinal& b) {
        return (a < b) ? a : b;
    }

    static Ordinal Supremum(const Ordinal& a, const Ordinal& b) {
        return (a < b) ? b : a;
    }
};


// 2. АЛГЕБРА ЛЕНИВЫХ СПИСКОВ (АЛГЕБРАИЧЕСКИЕ ОПЕРАЦИИ)
template <typename T>
class LazyListAlgebra {
public:
    // 2.1. СЛОЖЕНИЕ (Конкатенация)
    static DeferredSequence<T>* Add(DeferredSequence<T>* left, Sequence<T>* right) {
        if (!left || !right) throw std::invalid_argument("Algebra: operands cannot be null");
        return left->Concat(right);
    }

    // 2.2. УМНОЖЕНИЕ (Zip)
    template <typename U>
    static DeferredSequence<Duo<T, U>>* Multiply(DeferredSequence<T>* left, Sequence<U>* right) {
        if (!left || !right) throw std::invalid_argument("Algebra: operands cannot be null");
        return left->template Zip<U>(right);
    }

    // 2.3. ОГРАНИЧЕНИЕ (Глухая фильтрация)
    // ИСПОЛЬЗУЕМ ШАБЛОН Func, ЧТОБЫ КОМПИЛЯТОР НЕ ПУТАЛСЯ В УКАЗАТЕЛЯХ!
    template <typename Func>
    static DeferredSequence<T>* Restrict(DeferredSequence<T>* sequence, Func predicate) {
        if (!sequence) throw std::invalid_argument("Algebra: sequence is null");
        return sequence->Where(predicate);
    }

    // 2.4. ГОМОМОРФИЗМ (Трансформация Map)
    template <typename Func>
    static DeferredSequence<T>* Transform(DeferredSequence<T>* sequence, Func func) {
        if (!sequence) throw std::invalid_argument("Algebra: sequence is null");
        return sequence->Map(func);
    }

    // 2.5. ПРОЕКЦИЯ (Обрезание длины)
    static DeferredSequence<T>* Project(DeferredSequence<T>* sequence, int limit) {
        if (!sequence) throw std::invalid_argument("Algebra: sequence is null");
        return sequence->Take(limit);
    }

    // 2.6. СДВИГ (Пропуск элементов)
    static DeferredSequence<T>* Shift(DeferredSequence<T>* sequence, int offset) {
        if (!sequence) throw std::invalid_argument("Algebra: sequence is null");
        return sequence->Skip(offset);
    }
};

// =====================================================================
// 3. ПЕРЕГРУЗКА ОПЕРАТОРОВ (МАТЕМАТИЧЕСКИЙ СИНТАКСИС)
// =====================================================================

template <typename T>
DeferredSequence<T>* operator+(DeferredSequence<T>& left, Sequence<T>& right) {
    return LazyListAlgebra<T>::Add(&left, &right);
}

template <typename T, typename U>
DeferredSequence<Duo<T, U>>* operator*(DeferredSequence<T>& left, Sequence<U>& right) {
    return LazyListAlgebra<T>::template Multiply<U>(&left, &right);
}

// Теперь операторы тоже используют шаблон Func. Ошибок типов больше не будет!
template <typename T, typename Func>
DeferredSequence<T>* operator|(DeferredSequence<T>& sequence, Func predicate) {
    return LazyListAlgebra<T>::Restrict(&sequence, predicate);
}

template <typename T, typename Func>
DeferredSequence<T>* operator>>(DeferredSequence<T>& sequence, Func func) {
    return LazyListAlgebra<T>::Transform(&sequence, func);
}

#endif // LABA4_LAZY_ALGEBRA_H