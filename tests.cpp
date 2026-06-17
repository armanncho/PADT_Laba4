#include <gtest/gtest.h>
#include <stdexcept>

#include "Ordinal.h"
#include "LazySequence.h"
#include "Stream.h"
#include "Algebra.h"
#include "Generators.h"
#include "Laba2/MutableArraySequence.h"

// =====================================================================
// ВСПОМОГАТЕЛЬНЫЕ КЛАССЫ И ФУНКЦИИ ДЛЯ ТЕСТОВ
// =====================================================================

class CounterGenerator : public IGenerator<int> {
    int current;
public:
    explicit CounterGenerator(int start = 0) : current(start) {}
    ~CounterGenerator() override = default;

    int GetNext() override { return current++; }
    bool HasNext() const override { return true; }
    IGenerator<int>* Clone() const override { return new CounterGenerator(current); }
};

class ArithmeticGenerator : public IGenerator<int> {
private:
    int current_val;
    int step_val;
public:
    ArithmeticGenerator(int start, int step) : current_val(start), step_val(step) {}
    ~ArithmeticGenerator() override = default;

    int GetNext() override {
        int res = current_val;
        current_val += step_val;
        return res;
    }
    bool HasNext() const override { return true; }
    IGenerator<int>* Clone() const override {
        return new ArithmeticGenerator(current_val, step_val);
    }
};

int multiply_by_10(const int& x) { return x * 10; }
bool is_even(const int& x) { return x % 2 == 0; }

// =====================================================================
// 1. ТЕСТЫ ДЛЯ ORDINAL (Алгебра бесконечностей)
// =====================================================================

TEST(OrdinalTest, Initialization) {
    Ordinal finite(5);
    EXPECT_FALSE(finite.has_infinity());
    EXPECT_EQ(finite.get_shift(), 5);

    Ordinal inf = Ordinal::infinity();
    EXPECT_TRUE(inf.has_infinity());
    EXPECT_EQ(inf.get_w_scale(), 1);
    EXPECT_EQ(inf.get_shift(), 0);
}

TEST(OrdinalTest, Comparison) {
    Ordinal a(5);
    Ordinal b(10);
    Ordinal inf1(1, 0);
    Ordinal inf2(1, 5);
    Ordinal inf_2w(2, 0);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b < inf1);
    EXPECT_TRUE(inf1 < inf2);
    EXPECT_TRUE(inf2 < inf_2w);

    EXPECT_TRUE(a == Ordinal(5));
    EXPECT_TRUE(a != b);
}

TEST(OrdinalTest, ArithmeticAddition) {
    Ordinal a(5);
    Ordinal b(10);
    Ordinal w = Ordinal::infinity();

    EXPECT_TRUE((a + b) == Ordinal(15));
    EXPECT_TRUE((w + a) == Ordinal(1, 5));
    EXPECT_TRUE((a + w) == Ordinal(1, 0));
}

// =====================================================================
// 2. ТЕСТЫ ДЛЯ DEFERRED SEQUENCE
// =====================================================================

class DeferredSequenceTest : public ::testing::Test {
protected:
    DeferredSequence<int>* seq;

    void SetUp() override {
        seq = new DeferredSequence<int>(new CounterGenerator(0), Ordinal::infinity(), 100);
    }

    void TearDown() override {
        delete seq;
    }
};

TEST_F(DeferredSequenceTest, LazyEvaluationAndCaching) {
    EXPECT_EQ(seq->Get(5), 5);
    EXPECT_EQ(seq->GetCachedCount(), 6);
    EXPECT_EQ(seq->Get(2), 2);
}

TEST_F(DeferredSequenceTest, TakeCreatesFiniteSequence) {
    DeferredSequence<int>* finite_seq = seq->Take(5);

    EXPECT_FALSE(finite_seq->GetOrdinalLength().has_infinity());
    EXPECT_EQ(finite_seq->GetLength(), 5);
    EXPECT_EQ(finite_seq->Get(4), 4);

    EXPECT_THROW(finite_seq->Get(5), std::out_of_range);

    delete finite_seq;
}

TEST_F(DeferredSequenceTest, SkipAndGetFirst) {
    DeferredSequence<int>* skipped_seq = seq->Skip(10);

    EXPECT_EQ(skipped_seq->GetFirst(), 10);
    EXPECT_EQ(skipped_seq->Get(5), 15);

    delete skipped_seq;
}

// =====================================================================
// 3. ТЕСТЫ ДЛЯ LAZY ALGEBRA
// =====================================================================

TEST(LazyAlgebraTest, OperatorFilter) {
    auto* seq = new DeferredSequence<int>(new CounterGenerator(0), Ordinal::infinity(), 100);

    DeferredSequence<int>* even_seq = (*seq) | is_even;

    EXPECT_EQ(even_seq->GetFirst(), 0);
    EXPECT_EQ(even_seq->Get(1), 2);
    EXPECT_EQ(even_seq->Get(3), 6);

    delete even_seq;
    delete seq;
}

TEST(LazyAlgebraTest, OperatorTransform) {
    auto* seq = new DeferredSequence<int>(new CounterGenerator(1), Ordinal::infinity(), 100);

    DeferredSequence<int>* mapped_seq = (*seq) >> multiply_by_10;

    EXPECT_EQ(mapped_seq->Get(0), 10);
    EXPECT_EQ(mapped_seq->Get(2), 30);

    delete mapped_seq;
    delete seq;
}

TEST(LazyAlgebraTest, OperatorConcat) { // ИСПРАВЛЕНО: Только один чистый тест без дубликатов
    auto* seq1 = new DeferredSequence<int>(new ArithmeticGenerator(0, 1), Ordinal(5), 100);
    auto* seq2 = new DeferredSequence<int>(new ArithmeticGenerator(100, 1), Ordinal(3), 100);

    DeferredSequence<int>* combined = (*seq1) + (*seq2);

    EXPECT_EQ(combined->GetLength(), 8);
    EXPECT_EQ(combined->Get(4), 4);
    EXPECT_EQ(combined->Get(5), 100);
    EXPECT_EQ(combined->Get(6), 101);

    delete combined;
    delete seq2;
    delete seq1;
}

// =====================================================================
// 4. ТЕСТЫ ДЛЯ STREAM (Потоки чтения/записи)
// =====================================================================

TEST(StreamTest, ReadOnlyStreamEndOfStream) {
    auto* seq = new DeferredSequence<int>(new CounterGenerator(0), Ordinal(3), 100);
    ReadOnlyStream<int> stream(seq);

    stream.Open();
    EXPECT_FALSE(stream.IsEndOfStream());

    EXPECT_EQ(stream.Read(), 0);
    EXPECT_EQ(stream.Read(), 1);
    EXPECT_EQ(stream.Read(), 2);

    EXPECT_TRUE(stream.IsEndOfStream());
    EXPECT_THROW(stream.Read(), EndOfStream);

    stream.Close();
    delete seq;
}

TEST(StreamTest, ReadOnlyStreamSeek) {
    auto* seq = new DeferredSequence<int>(new CounterGenerator(0), Ordinal(10), 100);
    ReadOnlyStream<int> stream(seq);

    stream.Open();
    EXPECT_TRUE(stream.CanSeek());

    stream.Seek(5);
    EXPECT_EQ(stream.GetPosition(), 5);
    EXPECT_EQ(stream.Read(), 5);

    stream.Close();
    delete seq;
}

TEST(StreamTest, WriteOnlyStreamAppend) {
    MutableArraySequence<int> buffer;
    WriteOnlyStream<int> stream(&buffer);

    stream.Open();
    stream.Write(42);
    stream.Write(99);
    stream.Close();

    EXPECT_EQ(buffer.GetLength(), 2);
    EXPECT_EQ(buffer.Get(0), 42);
    EXPECT_EQ(buffer.Get(1), 99);
}

// =====================================================================
// ТОЧКА ВХОДА ДЛЯ ТЕСТОВ
// =====================================================================
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}