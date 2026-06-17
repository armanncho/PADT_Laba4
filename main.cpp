#include <iostream>
#include <stdexcept>
#include <thread>

#include "Ordinal.h"
#include "Generators.h"
#include "LazySequence.h"
#include "Stream.h"
#include "Laba2/MutableArraySequence.h"
#include "Laba2/Sequence.h"

#include "Algebra.h"

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

int map_multiply_two(const int& x) { return x * 2; }
bool filter_even(const int& x) { return x % 2 == 0; }

// Функции для Анализатора событий
bool filter_sensor_noise(const int& val) { return val >= 0; }
int map_to_severity(const int& val) {
    if (val > 80) return 3;
    if (val > 40) return 2;
    return 1;
}

void main_menu();
void deferred_sequence_menu();
void streams_menu();
void task2_demo(); // ПРОТОТИП ДЛЯ ЗАДАНИЯ 2

DeferredSequence<int>* build_test_sequence();
void demo_stream_read();
void demo_stream_seek();
void demo_stream_write();

int main() {
    main_menu();
    return 0;
}

void main_menu() {
    bool running = true;
    while(running) {
        std::cout << "\n=== LABA 4 - LAZY PIPELINES & STREAMS ===\n";
        std::cout << "1. Deferred Sequence\n";
        std::cout << "2. Streams (Read/Write)\n";
        std::cout << "3. [ ЗАДАНИЕ 2 ] Алгебра списков и ординалов\n";
        std::cout << "0. Exit\n";

        int choice;
        std::cout << "Choice: ";
        std::cin >> choice;

        switch(choice) {
            case 1: deferred_sequence_menu(); break;
            case 2: streams_menu(); break;
            case 3: task2_demo(); break; // ЗАПУСК ЗАДАНИЯ 2
            case 0: running = false; break;
            default: std::cout << "Invalid choice\n";
        }
    }
}

void deferred_sequence_menu() {
    DeferredSequence<int>* current_sequence = nullptr;
    bool running = true;

    while(running) {
        std::cout << "\n--- DEFERRED SEQUENCE MENU ---\n";
        if(current_sequence == nullptr)
            std::cout << "Status: NOT CREATED\n";

        std::cout << "1. Create Arithmetic Sequence\n";
        std::cout << "2. Show N elements\n";
        std::cout << "3. Get element by index\n";
        std::cout << "4. Map (*2)\n";
        std::cout << "5. Filter (even numbers)\n";
        std::cout << "6. Get Subsequence\n";
        std::cout << "7. Append element\n";
        std::cout << "8. Prepend element\n";
        std::cout << "9. Insert At\n";
        std::cout << "10. Concat with another\n";
        std::cout << "11. Get by Ordinal (w + n)\n";
        std::cout << "12. Show length\n";
        std::cout << "13. Delete sequence\n";
        std::cout << "0. Back\n";

        int choice;
        std::cout << "Choice: ";
        std::cin >> choice;

        try {
            switch(choice) {
                case 1: {
                    if(current_sequence != nullptr) delete current_sequence;
                    int start, step;
                    std::cout << "Start value: "; std::cin >> start;
                    std::cout << "Step: "; std::cin >> step;

                    auto* gen = new ArithmeticGenerator(start, step);
                    current_sequence = new DeferredSequence<int>(gen, Ordinal::infinity(), 100);
                    std::cout << "Sequence created.\n";
                    break;
                }
                case 2: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    int count;
                    std::cout << "Elements count: "; std::cin >> count;
                    for(int i = 0; i < count; i++) {
                        std::cout << current_sequence->Get(i) << " ";
                    }
                    std::cout << "\n";
                    break;
                }
                case 3: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    int index;
                    std::cout << "Index: "; std::cin >> index;
                    std::cout << "Element = " << current_sequence->Get(index) << "\n";
                    break;
                }
                case 4: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    DeferredSequence<int>* old_sequence = current_sequence;
                    current_sequence = current_sequence->Map(map_multiply_two);
                    delete old_sequence;
                    std::cout << "Mapped (*2).\n";
                    break;
                }
                case 5: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    DeferredSequence<int>* old_sequence = current_sequence;
                    current_sequence = current_sequence->Where(filter_even);
                    delete old_sequence;
                    std::cout << "Filtered (even only).\n";
                    break;
                }
                case 6: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    int start, end;
                    std::cout << "Start index: "; std::cin >> start;
                    std::cout << "End index: "; std::cin >> end;
                    DeferredSequence<int>* old_sequence = current_sequence;
                    current_sequence = current_sequence->GetSubsequence(start, end);
                    delete old_sequence;
                    std::cout << "Subsequence extracted.\n";
                    break;
                }
                case 7: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    int value;
                    std::cout << "Append value: "; std::cin >> value;

                    DeferredSequence<int>* old_sequence = current_sequence;
                    current_sequence = current_sequence->Append(value);
                    delete old_sequence;
                    break;
                }
                case 8: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    int value;
                    std::cout << "Prepend value: "; std::cin >> value;

                    DeferredSequence<int>* old_sequence = current_sequence;
                    current_sequence = current_sequence->Prepend(value);
                    delete old_sequence;
                    break;
                }
                case 9: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    int value, index;
                    std::cout << "Value: "; std::cin >> value;
                    std::cout << "Index: "; std::cin >> index;

                    DeferredSequence<int>* old_sequence = current_sequence;
                    current_sequence = current_sequence->InsertAt(value, index);
                    delete old_sequence;
                    break;
                }
                case 10: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    int start, step;
                    std::cout << "Second sequence start: "; std::cin >> start;
                    std::cout << "Second sequence step: "; std::cin >> step;
                    auto* gen2 = new ArithmeticGenerator(start, step);
                    auto* second = new DeferredSequence<int>(gen2, Ordinal::infinity(), 100);

                    DeferredSequence<int>* old_sequence = current_sequence;
                    current_sequence = current_sequence->Concat(second);
                    delete old_sequence;
                    std::cout << "Concatenated.\n";
                    break;
                }
                case 11: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    int omega, offset;
                    std::cout << "Omega count (w): "; std::cin >> omega;
                    std::cout << "Offset (shift): "; std::cin >> offset;
                    Ordinal target(omega, offset);
                    std::cout << "Element = " << current_sequence->Get(target) << "\n";
                    break;
                }
                case 12: {
                    if(!current_sequence) { std::cout << "Create first.\n"; break; }
                    Ordinal len = current_sequence->GetOrdinalLength();
                    std::cout << "Length: " << len.get_w_scale() << "w + " << len.get_shift() << "\n";
                    break;
                }
                case 13: {
                    if(current_sequence) {
                        delete current_sequence;
                        current_sequence = nullptr;
                        std::cout << "Deleted.\n";
                    }
                    break;
                }
                case 0: running = false; break;
                default: std::cout << "Invalid choice\n";
            }
        }
        catch(const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
    if(current_sequence) delete current_sequence;
}

void streams_menu() {
    bool running = true;
    while(running) {
        std::cout << "\n--- STREAMS MENU ---\n";
        std::cout << "1. Read from Stream\n";
        std::cout << "2. Seek in Stream\n";
        std::cout << "3. Write to Stream\n";
        std::cout << "0. Back\n";

        int choice;
        std::cout << "Choice: ";
        std::cin >> choice;

        try {
            switch(choice) {
                case 1: demo_stream_read(); break;
                case 2: demo_stream_seek(); break;
                case 3: demo_stream_write(); break;
                case 0: running = false; break;
                default: std::cout << "Invalid choice\n";
            }
        }
        catch(const std::exception& e) {
            std::cout << "Stream Error: " << e.what() << "\n";
        }
    }
}

DeferredSequence<int>* build_test_sequence() {
    int start, step;
    std::cout << "Init Base Sequence\nStart value: "; std::cin >> start;
    std::cout << "Step: "; std::cin >> step;
    auto* gen = new ArithmeticGenerator(start, step);
    return new DeferredSequence<int>(gen, Ordinal::infinity(), 100);
}

void demo_stream_read() {
    Sequence<int>* seq = build_test_sequence();
    ReadOnlyStream<int> stream(seq);

    stream.Open();
    int count;
    std::cout << "Read count: "; std::cin >> count;

    for(int i = 0; i < count; i++) {
        std::cout << stream.Read() << " ";
    }
    std::cout << "\n";
    stream.Close();
    delete seq;
}

void demo_stream_seek() {
    Sequence<int>* seq = build_test_sequence();
    ReadOnlyStream<int> stream(seq);

    stream.Open();
    int position;
    std::cout << "Seek position (index): "; std::cin >> position;

    stream.Seek(position);
    std::cout << "Value = " << stream.Read() << "\n";

    stream.Close();
    delete seq;
}

void demo_stream_write() {
    MutableArraySequence<int> buffer_seq;
    WriteOnlyStream<int> stream(&buffer_seq);

    stream.Open();
    int count;
    std::cout << "Elements to write: "; std::cin >> count;

    for(int i = 0; i < count; i++) {
        int value;
        std::cout << "Value #" << i + 1 << ": "; std::cin >> value;
        stream.Write(value);
    }
    stream.Close();

    std::cout << "\nData saved in Sequence:\n";
    for(int i = 0; i < buffer_seq.GetLength(); i++) {
        std::cout << buffer_seq.Get(i) << " ";
    }
    std::cout << "\n";
}

// =====================================================================
// РЕАЛИЗАЦИЯ ЗАДАНИЯ №2 (Анализатор событий с алгеброй)
// =====================================================================
void task2_demo() {
    std::cout << "\n--- ЗАДАНИЕ 2: АЛГЕБРА ЛЕНИВЫХ СПИСКОВ И ОРДИНАЛОВ ---\n";

    auto* base_gen = new ArithmeticGenerator(-20, 15);
    auto* raw_stream = new DeferredSequence<int>(base_gen, Ordinal::infinity(), 100);

    std::cout << "[АЛГЕБРА ОРДИНАЛОВ] Исходный поток бесконечен. Длина: "
              << raw_stream->GetOrdinalLength().get_w_scale() << "w\n";

    std::cout << "[АЛГЕБРА СПИСКОВ] Применяем операторы: Фильтр (|) и Гомоморфизм (>>)\n";

    // МАГИЯ АЛГЕБРЫ: используем твои перегруженные операторы!
    DeferredSequence<int>* clean_stream = (*raw_stream) | filter_sensor_noise;
    DeferredSequence<int>* severity_stream = (*clean_stream) >> map_to_severity;

    // Проекция: урезаем w до 12
    DeferredSequence<int>* final_stream = LazyListAlgebra<int>::Project(severity_stream, 12);

    std::cout << "[АЛГЕБРА ОРДИНАЛОВ] После Project(12) длина стала конечной: "
              << final_stream->GetOrdinalLength().get_shift() << "\n\n";

    std::cout << "[КОМПОНЕНТ] ЗАПУСК СИСТЕМЫ МОНИТОРИНГА...\n";
    std::cout << "========================================================\n";

    for (int i = 0; i < 15; ++i) { // Просим 15 тиков, но ординал остановит на 12
        if (!final_stream->GetOrdinalLength().has_infinity() && i >= final_stream->GetLength()) {
            std::cout << "[ СИСТЕМА ] Ленивый список исчерпан (достигнут предел Ординала).\n";
            break;
        }

        int event_val = final_stream->Get(i);

        std::cout << "[Tick " << i + 1 << "] Входящий код: " << event_val << "  ==>  Реакция: ";
        if (event_val == 3) std::cout << "!!! КРИТИЧЕСКАЯ ТРЕВОГА - ОСТАНОВКА !!!";
        else if (event_val == 2) std::cout << "! ВНИМАНИЕ - ПРОВЕРЬТЕ СИСТЕМУ !";
        else std::cout << "Штатный режим.";
        std::cout << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "========================================================\n";

    delete final_stream;
    delete severity_stream;
    delete clean_stream;
    delete raw_stream;
}