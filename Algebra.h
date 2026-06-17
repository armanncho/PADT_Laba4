#ifndef LABA4_ALGEBRA_H
#define LABA4_ALGEBRA_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

namespace algebraic_flow {

    struct Ordinal {
        bool is_infinite;
        int value;

        explicit Ordinal(int v = 0) : is_infinite(false), value(v) {}
        static Ordinal Infinity() { Ordinal o; o.is_infinite = true; return o; }

        bool operator>=(const Ordinal& other) const {
            if (is_infinite) return true;
            if (other.is_infinite) return false;
            return value >= other.value;
        }

        // Некоммутативное сложение Кантора
        static Ordinal add(const Ordinal& a, const Ordinal& b) {
            if (b.is_infinite) return b;
            if (a.is_infinite) return Ordinal::Infinity();
            return Ordinal(a.value + b.value);
        }
    };

    template <typename T>
    class IProvider {
    public:
        virtual ~IProvider() = default;
        virtual T provide() = 0;
        virtual bool has_more() const = 0;
        virtual IProvider<T>* clone() const = 0;
    };

    template <typename T>
    class LazyStream {
    private:
        IProvider<T>* m_provider;
        mutable std::vector<T> m_memo;
        mutable bool m_drained = false;

    public:
        LazyStream(IProvider<T>* prov) : m_provider(prov) {}
        ~LazyStream() { delete m_provider; }

        const T& get_at(int idx) const {
            while (!m_drained && (int)m_memo.size() <= idx) {
                if (m_provider->has_more()) m_memo.push_back(m_provider->provide());
                else m_drained = true;
            }
            if (idx < 0 || idx >= (int)m_memo.size()) throw std::out_of_range("Out of bounds");
            return m_memo[idx];
        }

        int get_count() const { return (int)m_memo.size(); }
    };

    // --- 3. АЛГЕБРАИЧЕСКИЙ РЕАКТОР (Обработка событий) ---
    template <typename Event, typename Response>
    class EventReactor {
    private:
        Response (*m_logic_unit)(const Event&); // Указатель на функцию-реактор

    public:
        explicit EventReactor(Response (*logic)(const Event&)) : m_logic_unit(logic) {}

        LazyStream<Response>* react(const LazyStream<Event>& input) const {

            return new LazyStream<Response>(new Transformer<Event, Response>(nullptr, m_logic_unit));

        }
    };

    // --- 4. ВИЗУАЛИЗАТОР (Интерфейс пользователя) ---
    class StreamVisualizer {
    public:
        template <typename E, typename R>
        static void render(const LazyStream<E>& events, const LazyStream<R>& responses, int count) {
            std::cout << "--- [Event Flow Visualization] ---" << std::endl;
            for (int i = 0; i < count; ++i) {
                try {
                    std::cout << "Event ID: " << i
                              << " | Raw: " << events.get_at(i)
                              << " => Res: " << responses.get_at(i) << std::endl;
                } catch (...) { break; }
            }
        }
    };

    // --- ВСПОМОГАТЕЛЬНЫЙ ТРАНСФОРМАТОР (Компонент Алгебры) ---
    template <typename T, typename U>
    class Transformer : public IProvider<U> {
        IProvider<T>* m_base;
        U (*m_rule)(const T&);
    public:
        Transformer(IProvider<T>* b, U (*r)(const T&)) : m_base(b), m_rule(r) {}
        ~Transformer() { delete m_base; }
        U provide() override { return m_rule(m_base->provide()); }
        bool has_more() const override { return m_base->has_more(); }
        IProvider<U>* clone() const override { return new Transformer<T, U>(m_base->clone(), m_rule); }
    };

} // namespace algebraic_flow
#endif //LABA4_ALGEBRA_H
