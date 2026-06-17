#ifndef LABA4_ORDINAL_H
#define LABA4_ORDINAL_H

#include <stdexcept>

class Ordinal {
private:
    int w_scale; // коэффициент при omega
    int shift;   // константный сдвиг

public:
    Ordinal() : w_scale(0), shift(0) {}

    Ordinal(int base_shift) : w_scale(0), shift(base_shift) {}

    Ordinal(int omega_multiplier, int base_shift)
            : w_scale(omega_multiplier), shift(base_shift) {}

    static Ordinal infinity() {
        return Ordinal(1, 0);
    }

    bool has_infinity() const {
        return w_scale > 0;
    }

    int get_w_scale() const {
        return w_scale;
    }

    int get_shift() const {
        return shift;
    }

    bool operator==(const Ordinal& rhs) const {
        return (this->w_scale == rhs.w_scale) && (this->shift == rhs.shift);
    }

    bool operator!=(const Ordinal& rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const Ordinal& rhs) const {
        if (this->w_scale != rhs.w_scale) {
            return this->w_scale < rhs.w_scale;
        }
        return this->shift < rhs.shift;
    }

    Ordinal operator+(const Ordinal& rhs) const {
        // Естественное поглощение: правая бесконечность зануляет левый сдвиг
        if (rhs.w_scale > 0) {
            return Ordinal(this->w_scale + rhs.w_scale, rhs.shift);
        }
        return Ordinal(this->w_scale, this->shift + rhs.shift);
    }

    Ordinal operator-(const Ordinal& rhs) const {
        if (this->w_scale < rhs.w_scale) {
            throw std::logic_error("Ordinal underflow: negative omega result");
        }

        if (this->w_scale == rhs.w_scale) {
            if (this->w_scale > 0) {
                throw std::logic_error("Undefined operation: omega subtraction");
            }
            if (this->shift < rhs.shift) {
                throw std::logic_error("Ordinal underflow: negative shift result");
            }
            return Ordinal(0, this->shift - rhs.shift);
        }

        return Ordinal(this->w_scale - rhs.w_scale, this->shift);
    }
};

#endif // LABA4_ORDINAL_H