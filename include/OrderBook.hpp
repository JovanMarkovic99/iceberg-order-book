#pragma once

#include "Limit.hpp"

#include <memory>
#include <map>
#include <vector>

namespace jvn 
{
    class OrderBook {
    public:
        using quantity_type = Order::quantity_type;
        using id_type = Order::id_type;
        using limit_type = Order::limit_type;

        struct Match {
            id_type buy_id;
            id_type sell_id;
            limit_type limit;
            quantity_type quantity;
        };
    private:
        using sell_map_type = std::map<limit_type, Limit>;
        using buy_map_type = std::map<limit_type, Limit, std::greater<limit_type>>;
    public:
        using sell_iterator = sell_map_type::iterator;
        using buy_iterator = buy_map_type::iterator;
        using sell_const_iterator = sell_map_type::const_iterator;
        using buy_const_iterator = buy_map_type::const_iterator;

    public:
        OrderBook() = default;
        OrderBook(const OrderBook&) = delete;
        OrderBook(OrderBook&&) = default;
        ~OrderBook() = default;

        OrderBook& operator=(const OrderBook&) = delete;
        OrderBook& operator=(OrderBook&&) = default;

        std::vector<Match> processOrder(std::unique_ptr<Order> order);

        ALWAYS_INLINE buy_const_iterator buyBegin() const noexcept;
        ALWAYS_INLINE sell_const_iterator sellBegin() const noexcept;
        ALWAYS_INLINE buy_const_iterator buyEnd() const noexcept;
        ALWAYS_INLINE sell_const_iterator sellEnd() const noexcept;
    private:
        sell_map_type m_sell_map;
        buy_map_type m_buy_map;

        template <class MapTy>
        static std::vector<Match> matchOrder(MapTy& map, Order* order);
        template <class MapTy>
        static void addOrder(MapTy& map, std::unique_ptr<Order> order);
        template <OrderType OrderTy>
        static void mergeIcebergs(std::vector<Match>& matches);
    };
}