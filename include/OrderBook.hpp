#pragma once

#include "Limit.hpp"
#include <memory>
#include <map>
#include <vector>
#include <iostream>

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
        OrderBook();

        std::vector<Match> processOrder(std::unique_ptr<Order> order);
    private:
        sell_map_type m_sell_map;
        buy_map_type m_buy_map;

        sell_iterator m_lowest_sell;
        buy_iterator m_highest_buy;
    };
}