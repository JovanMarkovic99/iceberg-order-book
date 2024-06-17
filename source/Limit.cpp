
#include "../include/Limit.hpp"

#include <cassert>

namespace jvn
{
    Limit::Limit(limit_type limit) :
        m_limit(limit) {
        assert(limit >= 0);
    }

    Order* Limit::front() {
        assert(isEmpty());
        return m_orders.front().get();
    }

    std::unique_ptr<Order> Limit::popFront() noexcept {
        assert(isEmpty());
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
        return !m_orders.size();
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