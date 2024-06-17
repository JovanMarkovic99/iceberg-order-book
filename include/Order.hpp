#pragma once

#include "Util.hpp"

namespace jvn 
{
    enum class OrderType {
        BUY,
        SELL
    };

    class Order {
    public:
        using limit_type = short;
        using quantity_type = int;
        using id_type = int;
        
    public:
        Order(OrderType order_type, id_type id, limit_type limit, quantity_type quantity);
        virtual ~Order() = default;

        ALWAYS_INLINE [[nodiscard]] OrderType getOrderType() const noexcept;
        ALWAYS_INLINE [[nodiscard]] id_type getId() const noexcept;
        ALWAYS_INLINE [[nodiscard]] limit_type getLimit() const noexcept;
        ALWAYS_INLINE [[nodiscard]] virtual quantity_type getVolume() const noexcept;

        ALWAYS_INLINE virtual void matchVolume(quantity_type volume) noexcept;
    protected:
        quantity_type m_quantity;
        id_type m_id;
        limit_type m_limit;
        OrderType m_order_type;
    };

    class IcebergOrder : public Order {
    public:
        IcebergOrder(OrderType order_type, id_type id, limit_type limit, 
                    quantity_type quantity, quantity_type peak);
        ~IcebergOrder() override = default;

        ALWAYS_INLINE [[nodiscard]] quantity_type getVolume() const noexcept final override;

        ALWAYS_INLINE void matchVolume(quantity_type volume) noexcept final override;
    private:
        quantity_type m_peak;
        quantity_type m_curr_peak;
    };
}