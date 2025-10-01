#pragma once

#include <iterator>

#include "types.h"

struct OrderNode {
    Order order;
    OrderNode *next;
};

namespace impl {
    class PriceLevelIterator;
}

template <class Allocator>
class PriceLevel {
public:
    using Iterator = impl::PriceLevelIterator;

    PriceLevel(Price price, Allocator &alloc);
    ~PriceLevel();

    PriceLevel(const PriceLevel&) = delete;
    PriceLevel(PriceLevel&&) = delete;
    PriceLevel& operator=(PriceLevel&&) = delete;
    PriceLevel& operator=(const PriceLevel&) = delete;

    [[nodiscard]] Price price() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] const Order& front() const noexcept;
    [[nodiscard]] Order& front() noexcept;

    void pushBack(const Order &order);
    void popFront();

    [[nodiscard]] Iterator begin() const noexcept;
    [[nodiscard]] Iterator end() const noexcept;

private:
    Price price_;
    OrderNode* head_{nullptr};
    OrderNode* tail_{nullptr};
    Allocator& alloc_;

};

namespace impl {
    class PriceLevelIterator {
    public:
        using value_type = Order;
        using reference = const Order&;
        using pointer = const Order*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

    public:
        PriceLevelIterator() = default;
        PriceLevelIterator(const PriceLevelIterator&) = default;
        PriceLevelIterator(PriceLevelIterator&&) = default;
        PriceLevelIterator& operator=(const PriceLevelIterator&) = default;
        PriceLevelIterator& operator=(PriceLevelIterator&&) = default;

        [[nodiscard]] reference operator*() const noexcept;
        [[nodiscard]] pointer operator->() const noexcept;

        PriceLevelIterator& operator++() noexcept;
        PriceLevelIterator operator++(int) noexcept;

        bool operator==(const PriceLevelIterator&) const = default;
        bool operator!=(const PriceLevelIterator&) const = default;

    private:
        explicit PriceLevelIterator(const OrderNode* node);

        template <class Allocator>
        friend class ::PriceLevel;

        const OrderNode* node_{nullptr};

    };
}

#include "price_level.inl"
