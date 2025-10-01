#include <algorithm>
#include <tuple>

#include "util.h"

ALWAYS_INLINE OrderBookSide::OrderBookSide(bool is_buy)
    : is_buy_(is_buy)
    , cmp_([is_buy](Price lhs, Price rhs) { return is_buy ? lhs > rhs : lhs < rhs; })
    , levels_map_(cmp_) {
}

ALWAYS_INLINE bool OrderBookSide::isBuy() const noexcept {
    return is_buy_;
}

ALWAYS_INLINE bool OrderBookSide::empty() const noexcept {
    return levels_map_.empty();
}

ALWAYS_INLINE auto OrderBookSide::comparator() const noexcept -> const Comparator& {
    return cmp_;
}

ALWAYS_INLINE Price OrderBookSide::bestPrice() const {
    assert(!empty());
    const auto& [price, level] = *(levels_map_.cbegin());
    return price;
}

ALWAYS_INLINE const Order& OrderBookSide::bestOrder() const {
    assert(!empty());
    const auto& [price, level] = *(levels_map_.cbegin());
    return level.front();
}

ALWAYS_INLINE void OrderBookSide::addOrder(const Order& order) {
    bool is_buy_order = order.side == Order::Side::BUY;
    assert(is_buy_order == isBuy());

    auto [iter, inserted] = levels_map_.try_emplace(order.price, order.price, node_alloc_);
    auto& [price, level] = *iter;
    return level.pushBack(order);
}

ALWAYS_INLINE Quantity OrderBookSide::consumeBest(Quantity qty) {
    auto& [price, level] = *(levels_map_.begin());
    auto& order = level.front();
    auto consumed = std::min(qty, order.visible_qty);
    order.visible_qty -= consumed;
    if (order.visible_qty == 0) {
        if (order.hidden_qty) {
            order.visible_qty = std::min(order.hidden_qty, order.peak_qty);
            order.hidden_qty -= order.visible_qty;
            level.pushBack(order);
            level.popFront();
        } else {
            level.popFront();
            if (level.empty())
                levels_map_.erase(levels_map_.begin());
        }
    }
    return consumed;
}

ALWAYS_INLINE auto OrderBookSide::begin() const noexcept -> Iterator {
    return Iterator{levels_map_, std::cbegin(levels_map_)};
}
ALWAYS_INLINE auto OrderBookSide::end() const noexcept -> Iterator {
    return Iterator{levels_map_, std::cend(levels_map_)};
}

ALWAYS_INLINE OrderBookSide::OrderBookSideIterator::OrderBookSideIterator(
    const LevelsMap& levels_map, LevelsMap::const_iterator start)
    : levels_map_(&levels_map)
    , current_(start) {
    if (current_ != std::cend(*levels_map_))
        level_current_ = current_->second.begin();
}

ALWAYS_INLINE auto OrderBookSide::OrderBookSideIterator::operator*() const noexcept -> reference {
    assert(current_ != std::cend(*levels_map_));
    assert(level_current_ != current_->second.end());
    return *level_current_;
}

ALWAYS_INLINE auto OrderBookSide::OrderBookSideIterator::operator->() const noexcept -> pointer {
    return &(operator*());
}

ALWAYS_INLINE auto OrderBookSide::OrderBookSideIterator::operator++() noexcept
-> OrderBookSideIterator& {
    if (++level_current_ != current_->second.end())
        return *this;

    level_current_ = (++current_ != std::cend(*levels_map_))
        ? current_->second.begin()
        : Level::Iterator{};
    return *this;
}

ALWAYS_INLINE auto OrderBookSide::OrderBookSideIterator::operator++(int) noexcept
-> OrderBookSideIterator {
    OrderBookSideIterator tmp(*this);
    ++(*this);
    return tmp;
}
