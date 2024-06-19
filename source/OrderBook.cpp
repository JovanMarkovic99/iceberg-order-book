
#include "../include/OrderBook.hpp"

#include <cassert>
#include <type_traits>
#include <unordered_map>

namespace jvn
{
    std::vector<OrderBook::Match> OrderBook::processOrder(std::unique_ptr<Order> order) {
        assert(order);

        std::vector<Match> matches;
        if (order->getOrderType() == OrderType::SELL) {
            matches = matchOrder(m_buy_map, order.get());
            mergeIcebergs<OrderType::SELL>(matches);

            if (order->getQuantity() > 0)
                addOrder(m_sell_map, std::move(order));
        } else {
            matches = matchOrder(m_sell_map, order.get());
            mergeIcebergs<OrderType::BUY>(matches);
            
            if (order->getQuantity() > 0)
                addOrder(m_buy_map, std::move(order));
        }

        return matches;
    }
    
    template <class MapTy>
    std::vector<OrderBook::Match> OrderBook::matchOrder(MapTy& map, Order* order) {
        std::vector<Match> matches;

        auto map_iter = map.begin();
        const auto is_matchable = [&]() {
            return map_iter != map.end() && !map.key_comp()(order->getLimit(), map_iter->first);
        };
        const auto top_limit = [&]() -> Limit& { return map_iter->second; };
        const auto front_order = [&]() -> Order* { return top_limit().front(); };
        const auto create_match = [&](quantity_type matching_volume) {
            if constexpr (std::is_same_v<MapTy, OrderBook::buy_map_type>)
                matches.emplace_back(Match{front_order()->getId(), 
                                        order->getId(), 
                                        map_iter->first,
                                        matching_volume});
            else
                matches.emplace_back(Match{order->getId(), 
                                        front_order()->getId(), 
                                        map_iter->first, 
                                        matching_volume});
        };

        if (!is_matchable())
            return matches;

        while (order->getQuantity()) {
            quantity_type front_volume = front_order()->getVolume();
            if (front_volume <= order->getQuantity()) {
                create_match(front_volume);

                order->matchQuantity(front_volume);

                auto exhausted_order = top_limit().popFront();
                exhausted_order->matchVolume(front_volume);

                // Non-exhausted iceberg order
                if (exhausted_order->getQuantity() > 0) {
                    assert(dynamic_cast<IcebergOrder*>(exhausted_order.get()));
                    top_limit().pushBack(std::move(exhausted_order));
                }
                else if (top_limit().isEmpty()) {
                    map_iter = map.erase(map_iter);
                    if (!is_matchable())
                        return matches;
                }
            } else {
                create_match(order->getQuantity());

                front_order()->matchVolume(order->getQuantity());

                order->matchQuantity(order->getQuantity());
                break;
            }
        }

        return matches;
    }

    template <class MapTy>
    void OrderBook::addOrder(MapTy& map, std::unique_ptr<Order> order) {
        if (order->getQuantity() == 0)
            return;

        auto [iter, inserted] = map.emplace(order->getLimit(), order->getLimit());
        iter->second.pushBack(std::move(order));
    }

    template <OrderType OrderTy>
    void OrderBook::mergeIcebergs(std::vector<Match>& matches) {
        std::unordered_map<id_type, size_t> seen_match_indices;
        auto id_match = [](const Match& m) {
            if constexpr (OrderTy == OrderType::BUY)
                return m.sell_id;
            else
                return m.buy_id;
        };

        size_t new_end = 0u;
        for (size_t idx = 0u; idx < matches.size(); ++idx) {
            auto [iter, inserted] = seen_match_indices.emplace(id_match(matches[idx]), new_end);
            if (inserted)
                matches[new_end++] = matches[idx];
            else
                matches[iter->second].quantity += matches[idx].quantity;
        }

        matches.resize(new_end);
    }
}