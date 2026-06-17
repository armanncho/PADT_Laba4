#ifndef DEFERRED_SEQUENCE_H
#define DEFERRED_SEQUENCE_H

#include "RollingWindow.h"
#include "Laba2/Sequence.h"
#include "Generators.h"
#include "Utils.h"
#include "Ordinal.h"
#include "Laba2/MutableArraySequence.h"

template<class T>
class DeferredSequence : public Sequence<T> {
private:
    mutable RollingWindow<T>* memory_window;
    IGenerator<T>* blueprint_generator;
    mutable IGenerator<T>* source_generator;

    Ordinal total_capacity;
    int max_cache_size;
    mutable int last_computed_idx;

    void validate_bounds(int target_index) const;

public:
    DeferredSequence();
    DeferredSequence(IGenerator<T>* blueprint, const Ordinal& cap, int cache_limit);
    ~DeferredSequence() override;

    const T& Get(const Ordinal& target_index) const;
    const T& Get(int target_index) const override;
    const T& GetFirst() const override;
    const T& GetLast() const override;

    // Для совместимости с Sequence
    int GetLength() const override { return total_capacity.get_shift(); }

    // Свой метод для бесконечности
    Ordinal GetOrdinalLength() const { return total_capacity; }

    int GetCachedCount() const;

    IEnumerator<T>* GetEnumerator() const override;
    Sequence<T>* Instance() override;
    Sequence<T>* CreateEmptySequence() const override;

    template<class Func>
    DeferredSequence<T>* Map(Func transform_fn);

    template<class Func>
    DeferredSequence<T>* Where(Func condition_fn);

    template<class Func>
    T Reduce(Func reduce_fn, T base_val);

    DeferredSequence<T>* Take(int amount);
    DeferredSequence<T>* Skip(int amount);
    DeferredSequence<T>* Concat(Sequence<T>* second_seq);
    DeferredSequence<T>* GetSubsequence(int from, int to);

    // Убрали override, чтобы не конфликтовать с возвращаемыми типами
    DeferredSequence<T>* Append(const T& element);
    DeferredSequence<T>* Prepend(const T& element);
    DeferredSequence<T>* InsertAt(const T& element, int target_index);

    template<class T2>
    DeferredSequence<Duo<T, T2>>* Zip(Sequence<T2>* second_seq);

    class DeferredEnumerator : public IEnumerator<T> {
    private:
        const DeferredSequence<T>* target_sequence;
        int step_index;

    public:
        explicit DeferredEnumerator(const DeferredSequence<T>* seq)
                : target_sequence(seq), step_index(0) {}

        bool has_more_elements() const override {
            Ordinal current_cap = target_sequence->GetOrdinalLength();
            if (current_cap.has_infinity()) return true;
            return step_index < current_cap.get_shift();
        }

        const T& next() override {
            return target_sequence->Get(step_index++);
        }
    };

protected:
    void AppendInternal(const T& element) override;
    void PrependInternal(const T& element) override;
    void InsertAtInternal(const T& element, int target_index) override;
    void RemoveAtInternal(int target_index) override;
};

// --- Реализация методов ---

template<class T>
DeferredSequence<T>::DeferredSequence()
        : memory_window(new RollingWindow<T>(100)),
          blueprint_generator(nullptr),
          source_generator(nullptr),
          last_computed_idx(-1),
          total_capacity(Ordinal::infinity()),
          max_cache_size(100)
{}

template<class T>
DeferredSequence<T>::DeferredSequence(IGenerator<T>* blueprint, const Ordinal& cap, int cache_limit)
        : memory_window(new RollingWindow<T>(cache_limit)),
          blueprint_generator(blueprint),
          source_generator(blueprint ? blueprint->Clone() : nullptr),
          last_computed_idx(-1),
          total_capacity(cap),
          max_cache_size(cache_limit)
{}

template<class T>
DeferredSequence<T>::~DeferredSequence() {
    delete source_generator;
    delete blueprint_generator;
    delete memory_window;
}

template<class T>
const T& DeferredSequence<T>::Get(int target_index) const {
    validate_bounds(target_index);

    if (memory_window->has_index(target_index)) {
        return memory_window->fetch(target_index);
    }

    if (target_index < last_computed_idx) {
        memory_window->reset();
        delete source_generator;
        source_generator = blueprint_generator->Clone();
        last_computed_idx = -1;
    }

    for (; last_computed_idx < target_index; ++last_computed_idx) {
        if (!source_generator || !source_generator->HasNext()) {
            throw std::out_of_range("Requested index exceeds generated bounds");
        }
        T next_val = source_generator->GetNext();
        memory_window->add(next_val, last_computed_idx + 1);
    }

    return memory_window->fetch(target_index);
}

template<class T>
const T& DeferredSequence<T>::Get(const Ordinal& target_index) const {
    if (target_index.has_infinity()) {
        throw std::logic_error("Cannot get element at infinite index");
    }
    return Get(target_index.get_shift());
}

template<class T>
const T& DeferredSequence<T>::GetFirst() const { return Get(0); }

template<class T>
const T& DeferredSequence<T>::GetLast() const {
    if (total_capacity.has_infinity()) {
        throw std::logic_error("Attempted to get the last element of an infinite sequence");
    }
    return Get(total_capacity.get_shift() - 1);
}

template<class T>
int DeferredSequence<T>::GetCachedCount() const { return memory_window->size(); }

template<class T>
IEnumerator<T>* DeferredSequence<T>::GetEnumerator() const {
    return new DeferredEnumerator(this);
}

template<class T>
Sequence<T>* DeferredSequence<T>::Instance() {
    throw std::logic_error("Copying operations for DeferredSequence are blocked");
}

template<class T>
Sequence<T>* DeferredSequence<T>::CreateEmptySequence() const {
    return new MutableArraySequence<T>();
}

template<class T>
DeferredSequence<T>* DeferredSequence<T>::Append(const T& element) {
    auto new_blueprint = new AppendGenerator<T>(this->blueprint_generator, element);
    Ordinal shifted_cap(total_capacity.get_w_scale(), total_capacity.get_shift() + 1);
    return new DeferredSequence<T>(new_blueprint, shifted_cap, max_cache_size);
}

template<class T>
DeferredSequence<T>* DeferredSequence<T>::Prepend(const T& element) {
    auto new_blueprint = new PrependGenerator<T>(this->blueprint_generator, element);
    Ordinal shifted_cap(total_capacity.get_w_scale(), total_capacity.get_shift() + 1);
    return new DeferredSequence<T>(new_blueprint, shifted_cap, max_cache_size);
}

template<class T>
DeferredSequence<T>* DeferredSequence<T>::InsertAt(const T& element, int target_index) {
    if (target_index < 0) throw std::out_of_range("Insert index cannot be negative");
    if (!total_capacity.has_infinity() && target_index > total_capacity.get_shift()) {
        throw std::out_of_range("Insert index goes beyond sequence length");
    }

    auto new_blueprint = new InsertAtGenerator<T>(this->blueprint_generator, element, target_index);
    Ordinal shifted_cap(total_capacity.get_w_scale(), total_capacity.get_shift() + 1);
    return new DeferredSequence<T>(new_blueprint, shifted_cap, max_cache_size);
}

template<class T>
void DeferredSequence<T>::AppendInternal(const T&) { throw std::logic_error("Internal appending forbidden"); }
template<class T>
void DeferredSequence<T>::PrependInternal(const T&) { throw std::logic_error("Internal prepending forbidden"); }
template<class T>
void DeferredSequence<T>::InsertAtInternal(const T&, int) { throw std::logic_error("Internal insertion forbidden"); }
template<class T>
void DeferredSequence<T>::RemoveAtInternal(int) { throw std::logic_error("Internal deletion forbidden"); }

template<class T>
template<class Func>
DeferredSequence<T>* DeferredSequence<T>::Map(Func transform_fn) {
    auto new_blueprint = new MapGenerator<T, T>(this->blueprint_generator, transform_fn);
    return new DeferredSequence<T>(new_blueprint, GetOrdinalLength(), max_cache_size);
}

template<class T>
template<class Func>
DeferredSequence<T>* DeferredSequence<T>::Where(Func condition_fn) {
    auto new_blueprint = new FilterGenerator<T>(this->blueprint_generator, condition_fn);
    return new DeferredSequence<T>(new_blueprint, Ordinal::infinity(), max_cache_size);
}

template<class T>
template<class Func>
T DeferredSequence<T>::Reduce(Func reduce_fn, T base_val) {
    T accum = base_val;
    IEnumerator<T>* it = GetEnumerator();
    for (; it->has_more_elements(); ) {
        accum = reduce_fn(accum, it->next());
    }
    delete it;
    return accum;
}

template<class T>
DeferredSequence<T>* DeferredSequence<T>::Take(int amount) {
    if (amount < 0) throw std::out_of_range("Take amount cannot be negative");

    auto new_blueprint = new TakeGenerator<T>(this->blueprint_generator, amount);
    return new DeferredSequence<T>(new_blueprint, Ordinal(amount), max_cache_size);
}

template<class T>
DeferredSequence<T>* DeferredSequence<T>::Skip(int amount) {
    if (amount < 0) throw std::out_of_range("Skip amount cannot be negative");

    auto new_blueprint = new SkipGenerator<T>(this->blueprint_generator, amount);

    int shifted_offset = total_capacity.get_shift() - amount;
    shifted_offset = (shifted_offset < 0) ? 0 : shifted_offset;

    Ordinal shifted_cap(total_capacity.get_w_scale(), shifted_offset);
    return new DeferredSequence<T>(new_blueprint, shifted_cap, max_cache_size);
}

template<class T>
DeferredSequence<T>* DeferredSequence<T>::Concat(Sequence<T>* second_seq) {
    auto new_blueprint = new ConcatGenerator<T>(this->blueprint_generator, second_seq);
    return new DeferredSequence<T>(new_blueprint, GetOrdinalLength() + second_seq->GetLength(), max_cache_size);
}

template<class T>
DeferredSequence<T>* DeferredSequence<T>::GetSubsequence(int from, int to) {
    if (from < 0 || to < from) throw std::out_of_range("Subsequence bounds are invalid");
    DeferredSequence<T>* temp = Skip(from);
    DeferredSequence<T>* res = temp->Take(to - from + 1);
    delete temp;
    return res;
}

template<class T>
template<class T2>
DeferredSequence<Duo<T, T2>>* DeferredSequence<T>::Zip(Sequence<T2>* second_seq) {
    auto new_blueprint = new ZipGenerator<T, T2>(this->blueprint_generator, second_seq);

    Ordinal final_cap;
    bool is_first_inf = this->GetOrdinalLength().has_infinity();
    bool is_second_inf = second_seq->GetLength().has_infinity(); // Полагаем, что second_seq может не быть отложенным

    if (is_first_inf && is_second_inf) {
        final_cap = Ordinal::infinity();
    }
    else if (!is_first_inf && !is_second_inf) {
        int min_offset = (this->GetLength() < second_seq->GetLength())
                         ? this->GetLength()
                         : second_seq->GetLength();
        final_cap = Ordinal(min_offset);
    }
    else {
        final_cap = is_first_inf ? Ordinal(second_seq->GetLength()) : this->GetOrdinalLength();
    }

    return new DeferredSequence<Duo<T, T2>>(new_blueprint, final_cap, max_cache_size);
}

template<class T>
void DeferredSequence<T>::validate_bounds(int target_index) const {
    if (target_index < 0) throw std::out_of_range("Target index is below zero");
    if (!total_capacity.has_infinity() && target_index >= total_capacity.get_shift()) {
        throw std::out_of_range("Target index is strictly out of bounds");
    }
}

#endif // DEFERRED_SEQUENCE_H