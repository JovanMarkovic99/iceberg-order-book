#pragma once

#include <memory>
#include <functional>
#include <map>

#include "price_level.h"

class OrderBookSide {
private:
    class OrderBookSideIterator;

public:
    using Iterator = OrderBookSideIterator;
    using Comparator = std::function<bool(Price, Price)>;

public:
    OrderBookSide(bool is_buy);
    ~OrderBookSide() = default;

    OrderBookSide(const OrderBookSide&) = delete;
    OrderBookSide(OrderBookSide&&) = delete;
    OrderBookSide& operator=(const OrderBookSide&) = delete;
    OrderBookSide& operator=(OrderBookSide&&) = delete;

    [[nodiscard]] bool isBuy() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const Comparator& comparator() const noexcept;
    [[nodiscard]] Price bestPrice() const;
    [[nodiscard]] const Order& bestOrder() const;

    void addOrder(const Order& order);
    Quantity consumeBest(Quantity qty);

    [[nodiscard]] Iterator begin() const noexcept;
    [[nodiscard]] Iterator end() const noexcept;

private:
    using NodeAllocator = std::allocator<OrderNode>;
    using Level = PriceLevel<NodeAllocator>;
    using LevelsMap = std::map<Price, Level, Comparator>;

    bool is_buy_;
    Comparator cmp_;
    NodeAllocator node_alloc_;
    LevelsMap levels_map_;

    class OrderBookSideIterator {
    public:
        using value_type = Level::Iterator::value_type;
        using reference = Level::Iterator::reference;
        using pointer = Level::Iterator::pointer;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

    public:
        OrderBookSideIterator(const OrderBookSideIterator&) = default;
        OrderBookSideIterator(OrderBookSideIterator&&) = default;
        OrderBookSideIterator& operator=(const OrderBookSideIterator&) = default;
        OrderBookSideIterator& operator=(OrderBookSideIterator&&) = default;

        [[nodiscard]] reference operator*() const noexcept;
        [[nodiscard]] pointer operator->() const noexcept;

        OrderBookSideIterator& operator++() noexcept;
        OrderBookSideIterator operator++(int) noexcept;

        bool operator==(const OrderBookSideIterator&) const noexcept;
        bool operator!=(const OrderBookSideIterator&) const noexcept;

    private:
        explicit OrderBookSideIterator(
            const LevelsMap& levels_map, LevelsMap::const_iterator start);

        friend class OrderBookSide;

        const LevelsMap& levels_map_;
        LevelsMap::const_iterator current_;
        Level::Iterator level_current_;

    };
};

#include "order_book_side.inl"
