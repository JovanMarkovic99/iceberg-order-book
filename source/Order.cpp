
#include "../include/Order.hpp"

#include <cassert>
#include <algorithm>

namespace jvn
{
    Order::Order(OrderType order_type, id_type id, limit_type limit, quantity_type quantity) :
        m_order_type(order_type),
        m_id(id),
        m_limit(limit),
        m_quantity(quantity) {
        assert(quantity >= 0);
        assert(limit >= 0);
    }

    IcebergOrder::IcebergOrder(OrderType order_type, id_type id, limit_type limit, 
                            quantity_type quantity, quantity_type peak) :
        Order(order_type, id, limit, quantity),
        m_peak(peak),
        m_curr_peak(std::min(quantity, peak)) {
        assert(peak > 0);
    }
}