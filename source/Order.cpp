
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

    OrderType Order::getOrderType() const noexcept {
        return m_order_type;
    }

    Order::id_type Order::getId() const noexcept {
        return m_id;
    }

    Order::limit_type Order::getLimit() const noexcept {
        return m_limit;
    }

    Order::quantity_type Order::getVolume() const noexcept {
        return m_quantity;
    }
    
    void Order::matchVolume(quantity_type volume) noexcept {
        assert(volume > 0 && volume <= m_quantity);
        m_quantity -= volume;
    }

    IcebergOrder::IcebergOrder(OrderType order_type, id_type id, limit_type limit, 
                            quantity_type quantity, quantity_type peak) :
        Order(order_type, id, limit, quantity),
        m_peak(peak),
        m_curr_peak(std::min(quantity, peak)) {
        assert(peak > 0);
    }

    Order::quantity_type IcebergOrder::getVolume() const noexcept {
        return m_curr_peak;
    }

    void IcebergOrder::matchVolume(quantity_type volume) noexcept {
        Order::matchVolume(volume);

        assert(m_curr_peak >= volume);
        m_curr_peak -= volume;
        if (m_curr_peak == 0)
            m_curr_peak = std::min(m_quantity, m_peak);
    }
}