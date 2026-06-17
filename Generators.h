#ifndef LABA4_GENERATORS_H
#define LABA4_GENERATORS_H

#include <stdexcept>
#include "Laba2/Sequence.h"
#include "Laba2/Option.h"
#include "Ordinal.h"
#include "Utils.h"

template <typename T>
class IGenerator {
public:
    virtual ~IGenerator() = default;

    virtual T GetNext() = 0;
    virtual bool HasNext() const = 0;
    virtual IGenerator<T>* Clone() const = 0;

    virtual Option<T> TryGetNext() {
        if (HasNext()) return Option<T>(GetNext());
        return Option<T>();
    }
};

//  RECURRENT GENERATOR
template<class T>
class RuleGenerator : public IGenerator<T> {
public:
    typedef T (*RuleFunc)(const Sequence<T>*);

    RuleGenerator(RuleFunc rule, const Sequence<T>* materialized, Ordinal limit = Ordinal::infinity());
    T GetNext() override;
    bool HasNext() const override;
    IGenerator<T>* Clone() const override;

private:
    RuleFunc rule;
    const Sequence<T>* materialized;
    Ordinal limit;
    int current_count;
};

template<class T>
RuleGenerator<T>::RuleGenerator(RuleFunc rule, const Sequence<T>* materialized, Ordinal limit)
        : rule(rule), materialized(materialized), limit(limit), current_count(0) {
    if (!rule) throw std::logic_error("Rule function cannot be null");
}

template<class T>
T RuleGenerator<T>::GetNext() {
    if (!HasNext()) throw std::out_of_range("RuleGenerator reached its limit");
    T next_element = rule(materialized);
    current_count++;
    return next_element;
}

template<class T>
bool RuleGenerator<T>::HasNext() const {
    if (limit.has_infinity()) return true;
    return current_count < limit.get_shift();
}

template<class T>
IGenerator<T>* RuleGenerator<T>::Clone() const {
    auto* clone = new RuleGenerator<T>(rule, materialized, limit);
    clone->current_count = this->current_count;
    return clone;
}

// APPEND GENERATOR
template<class T>
class AppendGenerator : public IGenerator<T> {
public:
    AppendGenerator(IGenerator<T>* source, const T& item, bool yielded = false);
    ~AppendGenerator() override;
    T GetNext() override;
    bool HasNext() const override;
    IGenerator<T>* Clone() const override;
private:
    IGenerator<T>* source;
    T appended_item;
    bool item_yielded;
};

template<class T>
AppendGenerator<T>::AppendGenerator(IGenerator<T>* source, const T& item, bool yielded)
        : source(source ? source->Clone() : nullptr), appended_item(item), item_yielded(yielded) {}

template<class T>
AppendGenerator<T>::~AppendGenerator() { delete source; }

template<class T>
T AppendGenerator<T>::GetNext() {
    if (source && source->HasNext()) {
        return source->GetNext();
    }
    if (!item_yielded) {
        item_yielded = true;
        return appended_item;
    }
    throw std::out_of_range("AppendGenerator: exhausted");
}

template<class T>
bool AppendGenerator<T>::HasNext() const {
    return (source && source->HasNext()) || !item_yielded;
}

template<class T>
IGenerator<T>* AppendGenerator<T>::Clone() const {
    return new AppendGenerator<T>(source, appended_item, item_yielded);
}

// PREPEND GENERATOR
template<class T>
class PrependGenerator : public IGenerator<T> {
public:
    PrependGenerator(IGenerator<T>* source, const T& item, bool yielded = false)
            : source(source ? source->Clone() : nullptr), prepended_item(item), item_yielded(yielded) {}

    ~PrependGenerator() override { delete source; }

    T GetNext() override {
        if (!item_yielded) {
            item_yielded = true;
            return prepended_item;
        }
        if (source && source->HasNext()) return source->GetNext();
        throw std::out_of_range("PrependGenerator is empty");
    }

    bool HasNext() const override {
        return !item_yielded || (source && source->HasNext());
    }

    IGenerator<T>* Clone() const override {
        return new PrependGenerator<T>(source, prepended_item, item_yielded);
    }
private:
    IGenerator<T>* source;
    T prepended_item;
    bool item_yielded;
};

// INSERT AT GENERATOR
template<class T>
class InsertAtGenerator : public IGenerator<T> {
public:
    InsertAtGenerator(IGenerator<T>* source, const T& item, int insert_index, int current = 0);
    ~InsertAtGenerator() override;
    T GetNext() override;
    bool HasNext() const override;
    IGenerator<T>* Clone() const override;
private:
    IGenerator<T>* source;
    T inserted_item;
    int insert_index;
    int current_index;
};

template<class T>
InsertAtGenerator<T>::InsertAtGenerator(IGenerator<T>* source, const T& item, int insert_index, int current)
        : source(source ? source->Clone() : nullptr), inserted_item(item), insert_index(insert_index), current_index(current) {}

template<class T>
InsertAtGenerator<T>::~InsertAtGenerator() { delete source; }

template<class T>
T InsertAtGenerator<T>::GetNext() {
    if (!HasNext()) throw std::out_of_range("InsertAtGenerator: exhausted");
    if (current_index == insert_index) {
        current_index++;
        return inserted_item;
    }
    current_index++;
    return source->GetNext();
}

template<class T>
bool InsertAtGenerator<T>::HasNext() const {
    return (source && source->HasNext()) || (current_index <= insert_index);
}

template<class T>
IGenerator<T>* InsertAtGenerator<T>::Clone() const {
    return new InsertAtGenerator<T>(source, inserted_item, insert_index, current_index);
}

// CONCAT GENERATOR
template<class T>
class ConcatGenerator : public IGenerator<T> {
public:
    ConcatGenerator(IGenerator<T>* first_source, Sequence<T>* second_sequence)
            : first_gen(first_source ? first_source->Clone() : nullptr), second_seq(second_sequence),
              second_enum(nullptr), first_exhausted(false), steps_in_second(0) {}

    ConcatGenerator(IGenerator<T>* first_source, Sequence<T>* second_sequence, bool exhausted, int steps)
            : first_gen(first_source ? first_source->Clone() : nullptr), second_seq(second_sequence),
              second_enum(nullptr), first_exhausted(exhausted), steps_in_second(steps) {
        if (first_exhausted && second_seq) {
            second_enum = second_seq->GetEnumerator();
            for (int i = 0; i < steps_in_second; ++i) {
                if (second_enum->has_more_elements()) second_enum->next();
            }
        }
    }

    ~ConcatGenerator() override {
        delete first_gen;
        if (second_enum) delete second_enum;
    }

    T GetNext() override {
        if (!first_exhausted) {
            if (first_gen && first_gen->HasNext()) return first_gen->GetNext();
            first_exhausted = true;
            if (second_seq) second_enum = second_seq->GetEnumerator();
        }
        if (second_enum && second_enum->has_more_elements()) {
            steps_in_second++;
            return second_enum->next();
        }
        throw std::out_of_range("ConcatGenerator is fully exhausted");
    }

    bool HasNext() const override {
        if (!first_exhausted && first_gen && first_gen->HasNext()) return true;
        if (!first_exhausted && second_seq) {
            IEnumerator<T>* temp = second_seq->GetEnumerator();
            bool has = temp->has_more_elements();
            delete temp;
            return has;
        }
        return second_enum && second_enum->has_more_elements();
    }

    IGenerator<T>* Clone() const override {
        return new ConcatGenerator<T>(first_gen, second_seq, first_exhausted, steps_in_second);
    }

private:
    IGenerator<T>* first_gen;
    Sequence<T>* second_seq;
    IEnumerator<T>* second_enum;
    bool first_exhausted;
    int steps_in_second;
};

//  WHERE GENERATOR
template<class T>
class FilterGenerator : public IGenerator<T> {
public:
    typedef bool (*PredicateFunc)(const T&);
    FilterGenerator(IGenerator<T>* source, PredicateFunc predicate);
    FilterGenerator(IGenerator<T>* source, PredicateFunc predicate, Option<T> current_buffer);
    ~FilterGenerator() override;
    T GetNext() override;
    bool HasNext() const override;
    IGenerator<T>* Clone() const override;
private:
    IGenerator<T>* source;
    PredicateFunc predicate;
    mutable Option<T> buffer;
    void FillBuffer() const;
};

template<class T>
FilterGenerator<T>::FilterGenerator(IGenerator<T>* source, PredicateFunc predicate)
        : source(source ? source->Clone() : nullptr), predicate(predicate), buffer(Option<T>()) {}

template<class T>
FilterGenerator<T>::FilterGenerator(IGenerator<T>* source, PredicateFunc predicate, Option<T> current_buffer)
        : source(source ? source->Clone() : nullptr), predicate(predicate), buffer(current_buffer) {}

template<class T>
FilterGenerator<T>::~FilterGenerator() { delete source; }

template<class T>
void FilterGenerator<T>::FillBuffer() const {
    if (buffer.HasValue() || !source) return;
    while (source->HasNext()) {
        T value = source->GetNext();
        if (predicate(value)) {
            buffer = Option<T>(value);
            return;
        }
    }
}

template<class T>
T FilterGenerator<T>::GetNext() {
    FillBuffer();
    if (!buffer.HasValue()) throw std::out_of_range("FilterGenerator: empty");
    T val = buffer.GetValue();
    buffer = Option<T>();
    return val;
}

template<class T>
bool FilterGenerator<T>::HasNext() const {
    FillBuffer();
    return buffer.HasValue();
}

template<class T>
IGenerator<T>* FilterGenerator<T>::Clone() const {
    return new FilterGenerator<T>(source, predicate, buffer);
}

//  MAP GENERATOR
template <typename SourceT, typename TargetT>
class MapGenerator : public IGenerator<TargetT> {
public:
    typedef TargetT (*TransformFunc)(const SourceT&);
    MapGenerator(IGenerator<SourceT>* src, TransformFunc f);
    ~MapGenerator() override;
    TargetT GetNext() override;
    bool HasNext() const override;
    IGenerator<TargetT>* Clone() const override;
private:
    IGenerator<SourceT>* source;
    TransformFunc func;
};

template<class SourceT, class TargetT>
MapGenerator<SourceT, TargetT>::MapGenerator(IGenerator<SourceT>* src, TransformFunc f)
        : source(src ? src->Clone() : nullptr), func(f) {}

template<class SourceT, class TargetT>
MapGenerator<SourceT, TargetT>::~MapGenerator() { delete source; }

template<class SourceT, class TargetT>
TargetT MapGenerator<SourceT, TargetT>::GetNext() { return func(source->GetNext()); }

template<class SourceT, class TargetT>
bool MapGenerator<SourceT, TargetT>::HasNext() const { return source && source->HasNext(); }

template<class SourceT, class TargetT>
IGenerator<TargetT>* MapGenerator<SourceT, TargetT>::Clone() const {
    return new MapGenerator<SourceT, TargetT>(source, func);
}

//  ZIP GENERATOR
template<class T1, class T2>
class ZipGenerator : public IGenerator<Duo<T1, T2>> {
public:
    ZipGenerator(IGenerator<T1>* source1, IGenerator<T2>* source2);
    ~ZipGenerator() override;
    Duo<T1, T2> GetNext() override;
    bool HasNext() const override;
    IGenerator<Duo<T1, T2>>* Clone() const override;
private:
    IGenerator<T1>* source1;
    IGenerator<T2>* source2;
};

template<class T1, class T2>
ZipGenerator<T1, T2>::ZipGenerator(IGenerator<T1>* source1, IGenerator<T2>* source2)
        : source1(source1 ? source1->Clone() : nullptr), source2(source2 ? source2->Clone() : nullptr) {}

template<class T1, class T2>
ZipGenerator<T1, T2>::~ZipGenerator() {
    delete source1;
    delete source2;
}

template<class T1, class T2>
Duo<T1, T2> ZipGenerator<T1, T2>::GetNext() {
    if (!HasNext()) throw std::out_of_range("ZipGenerator: exhausted");
    return Duo<T1, T2>(source1->GetNext(), source2->GetNext());
}

template<class T1, class T2>
bool ZipGenerator<T1, T2>::HasNext() const {
    return source1 && source2 && source1->HasNext() && source2->HasNext();
}

template<class T1, class T2>
IGenerator<Duo<T1, T2>>* ZipGenerator<T1, T2>::Clone() const {
    return new ZipGenerator<T1, T2>(source1, source2);
}

//  TAKE GENERATOR
template<class T>
class TakeGenerator : public IGenerator<T> {
public:
    TakeGenerator(IGenerator<T>* source, int limit, int current = 0);
    ~TakeGenerator() override;
    T GetNext() override;
    bool HasNext() const override;
    IGenerator<T>* Clone() const override;
private:
    IGenerator<T>* source;
    int limit;
    int produced;
};

template<class T>
TakeGenerator<T>::TakeGenerator(IGenerator<T>* source, int limit, int current)
        : source(source ? source->Clone() : nullptr), limit(limit), produced(current) {}

template<class T>
TakeGenerator<T>::~TakeGenerator() { delete source; }

template<class T>
T TakeGenerator<T>::GetNext() {
    if (!HasNext()) throw std::out_of_range("TakeGenerator: exhausted");
    produced++;
    return source->GetNext();
}

template<class T>
bool TakeGenerator<T>::HasNext() const {
    return (produced < limit) && source && source->HasNext();
}

template<class T>
IGenerator<T>* TakeGenerator<T>::Clone() const {
    return new TakeGenerator<T>(source, limit, produced);
}

// SKIP GENERATOR
template<class T>
class SkipGenerator : public IGenerator<T> {
public:
    SkipGenerator(IGenerator<T>* source, int skip_count, bool is_skipped = false);
    ~SkipGenerator() override;
    T GetNext() override;
    bool HasNext() const override;
    IGenerator<T>* Clone() const override;
private:
    IGenerator<T>* source;
    int skip_count;
    mutable bool is_skipped;
    void PerformSkip() const;
};

template<class T>
SkipGenerator<T>::SkipGenerator(IGenerator<T>* source, int skip_count, bool is_skipped)
        : source(source ? source->Clone() : nullptr), skip_count(skip_count), is_skipped(is_skipped) {}

template<class T>
SkipGenerator<T>::~SkipGenerator() { delete source; }

template<class T>
void SkipGenerator<T>::PerformSkip() const {
    if (is_skipped || !source) return;
    int dropped = 0;
    while (dropped < skip_count && source->HasNext()) {
        source->GetNext();
        dropped++;
    }
    is_skipped = true;
}

template<class T>
T SkipGenerator<T>::GetNext() {
    PerformSkip();
    if (!HasNext()) throw std::out_of_range("SkipGenerator: elements exhausted");
    return source->GetNext();
}

template<class T>
bool SkipGenerator<T>::HasNext() const {
    PerformSkip();
    return source && source->HasNext();
}

template<class T>
IGenerator<T>* SkipGenerator<T>::Clone() const {
    return new SkipGenerator<T>(source, skip_count, is_skipped);
}

#endif // LABA4_GENERATORS_H