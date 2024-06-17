
#include "../include/OrderBook.hpp"

#include <cassert>

namespace jvn
{
    std::vector<OrderBook::Match> OrderBook::processOrder(std::unique_ptr<Order> order) {
        assert(order);

        std::vector<Match> matches;
        if (order->getOrderType() == OrderType::BUY) {
            matches = matchOrder(m_buy_map, order.get());
            mergeIcebergs<OrderType::BUY>(matches);

            if (order->getQuantity() > 0)
                addOrder(m_buy_map, std::move(order));
        } else {
            matches = matchOrder(m_buy_map, order.get());
            mergeIcebergs<OrderType::BUY>(matches);
            
            if (order->getQuantity() > 0)
                addOrder(m_buy_map, std::move(order));
        }

        return matches;
    }

    // TODO
}