#include <utility>
#include <tuple>

#include "util.h"

ALWAYS_INLINE MatchingEngine::MatchingEngine()
    : buy_side_(true)
    , sell_side_(false) {
}

ALWAYS_INLINE const std::vector<Trade>& MatchingEngine::process(Order aggressive_order) {
    trades_.clear();
    passive_id_idx_.clear();
    auto [contra_side, same_side] = (aggressive_order.side == Order::Side::BUY)
        ? std::tie(sell_side_, buy_side_)
        : std::tie(buy_side_, sell_side_);
    match(aggressive_order, contra_side);
    if (aggressive_order.visible_qty)
        same_side.addOrder(aggressive_order);
    return trades_;
}

ALWAYS_INLINE void MatchingEngine::match(Order& aggressive_order, OrderBookSide& contra_side) {
    bool is_buy_order = aggressive_order.side == Order::Side::BUY;
    assert(is_buy_order != contra_side.isBuy());

    const auto& cmp = contra_side.comparator();
    auto aggressive_qty = aggressive_order.visible_qty + aggressive_order.hidden_qty;
    while (aggressive_qty
        && !contra_side.empty() && !cmp(aggressive_order.price, contra_side.bestPrice())) {
        auto level_price = contra_side.bestPrice();
        do {
            auto passive_order = contra_side.bestOrder();
            auto trade_qty = std::min(aggressive_qty, passive_order.visible_qty);
            aggressive_qty -= trade_qty;
            contra_side.consumeBest(trade_qty);
            addTrade(aggressive_order, passive_order, trade_qty);
        } while (aggressive_qty && !contra_side.empty() && contra_side.bestPrice() == level_price);
    }

    aggressive_order.visible_qty = std::min(aggressive_qty, aggressive_order.initial_visible_qty);
    aggressive_order.hidden_qty = aggressive_qty - aggressive_order.visible_qty;
}

ALWAYS_INLINE void MatchingEngine::addTrade(
    const Order& aggressive_order, const Order& passive_order, Quantity qty) {
    assert(aggressive_order.side != passive_order.side);

    auto [iter, inserted] = passive_id_idx_.emplace(passive_order.id, trades_.size());
    if (!inserted) {
        trades_[iter->second].qty += qty;
        return;
    }

    auto [aggresiveIdField, passiveIdField] = (aggressive_order.side == Order::Side::BUY)
        ? std::pair{&Trade::buy_id, &Trade::sell_id}
        : std::pair{&Trade::sell_id, &Trade::buy_id};
    Trade trade;
    trade.*aggresiveIdField = aggressive_order.id;
    trade.*passiveIdField = passive_order.id;
    trade.price = passive_order.price;
    trade.qty = qty;
    trades_.emplace_back(trade);
}

ALWAYS_INLINE auto MatchingEngine::buyBegin() const noexcept -> Iterator {
    return buy_side_.begin();
}

ALWAYS_INLINE auto MatchingEngine::buyEnd() const noexcept -> Iterator {
    return buy_side_.end();
}

ALWAYS_INLINE auto MatchingEngine::sellBegin() const noexcept -> Iterator {
    return sell_side_.begin();
}

ALWAYS_INLINE auto MatchingEngine::sellEnd() const noexcept -> Iterator {
    return sell_side_.end();
}
