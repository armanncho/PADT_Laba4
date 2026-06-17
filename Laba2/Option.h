#ifndef LABA2_OPTION_H
#define LABA2_OPTION_H

#include <stdexcept>

template <class T>
class Option {
private:
    bool has_value; // Флаг: есть ли внутри реальные данные
    T value;        // Само значение (если оно есть)

public:
    // 1. Конструктор для пустого значения (аналог None)
    Option() : has_value(false) {}

    // 2. Конструктор для успешного результата (аналог Some)
    Option(const T& val) : has_value(true), value(val) {}

    // Проверка: есть ли значение внутри?
    bool HasValue() const {
        return has_value;
    }

    // Безопасное извлечение значения
    const T& GetValue() const {
        if (!has_value) {
            throw std::logic_error("Attempted to get value from an empty Option");
        }
        return value;
    }
};

#endif // LABA2_OPTION_H