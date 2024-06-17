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
}