#pragma once

#include <vector>

#include "order_book_side.h"

class MatchingEngine {
public:
    using Iterator = OrderBookSide::Iterator;

public:
    MatchingEngine();
    ~MatchingEngine() = default;

    MatchingEngine(MatchingEngine&) = delete;
    MatchingEngine(MatchingEngine&&) = delete;
    MatchingEngine& operator=(MatchingEngine&) = delete;
    MatchingEngine& operator=(MatchingEngine&&) = delete;

    const std::vector<Trade>& process(Order aggressive_order);

    Iterator buyBegin() const noexcept;
    Iterator buyEnd() const noexcept;

    Iterator sellBegin() const noexcept;
    Iterator sellEnd() const noexcept;

private:
    OrderBookSide buy_side_;
    OrderBookSide sell_side_;
    std::vector<Trade> trades_;
    std::unordered_map<OrderId, size_t> passive_id_idx_;

    void match(Order& aggressive_order, OrderBookSide& contra_side);

    void addTrade(const Order& aggressive_order, const Order& passive_order, Quantity qty);

};

#include "matching_engine.inl"
