#pragma once

#include "Util.hpp"
#include "Order.hpp"

#include <memory>
#include <deque>

namespace jvn 
{
    class Limit {
    public:
        using limit_type = Order::limit_type;
        using list_type = std::deque<std::unique_ptr<Order>>;
        using iterator = list_type::iterator;
        using const_iterator = list_type::const_iterator;
        
    public:
        Limit(limit_type limit);
        Limit(const Limit&) = delete;
        Limit(Limit&&) = default;
        ~Limit() = default;

        Limit& operator=(const Limit&) = delete;
        Limit& operator=(Limit&&) = default;

        ALWAYS_INLINE [[nodiscard]] Order* front();
        ALWAYS_INLINE [[nodiscard]] const Order* front() const;
        ALWAYS_INLINE std::unique_ptr<Order> popFront();

        ALWAYS_INLINE void pushBack(std::unique_ptr<Order> order) noexcept;

        ALWAYS_INLINE [[nodiscard]] limit_type getLimit() const noexcept;
        ALWAYS_INLINE [[nodiscard]] bool isEmpty() const noexcept;
    
        ALWAYS_INLINE iterator begin() noexcept;
        ALWAYS_INLINE const_iterator begin() const noexcept;
        ALWAYS_INLINE iterator end() noexcept;
        ALWAYS_INLINE const_iterator end() const noexcept;
    private:
        limit_type m_limit;
        list_type m_orders;
    };


    Order* Limit::front() {
        assert(!isEmpty());
        return m_orders.front().get();
    }

    const Order* Limit::front() const {
        assert(!isEmpty());
        return m_orders.front().get();
    }

    std::unique_ptr<Order> Limit::popFront() {
        assert(!isEmpty());
        std::unique_ptr<Order> front_order = std::move(m_orders.front());
        m_orders.pop_front();
        return front_order;
    }

    void Limit::pushBack(std::unique_ptr<Order> order) noexcept {
        assert(order && order->getLimit() == m_limit);
        m_orders.emplace_back(std::move(order));
    }

    Limit::limit_type Limit::getLimit() const noexcept {
        return m_limit;
    }

    bool Limit::isEmpty() const noexcept {
        return m_orders.empty();
    }

    Limit::iterator Limit::begin() noexcept {
        return m_orders.begin();
    }

    Limit::iterator Limit::end() noexcept {
        return m_orders.end();
    }

    Limit::const_iterator Limit::begin() const noexcept {
        return m_orders.begin();
    }

    Limit::const_iterator Limit::end() const noexcept {
        return m_orders.end();
    }
}