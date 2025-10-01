#pragma once

#include <cstdint>

using OrderId = int;
using Quantity = int;
using Price = short;

struct Order {
    enum class Side { BUY, SELL };
    enum class Type { LIMIT, ICEBERG };

    Side side;
    Type type;
    OrderId id;
    Price price;
    Quantity visible_qty;
    Quantity initial_visible_qty;
    Quantity hidden_qty;
};

struct Trade {
    OrderId buy_id;
    OrderId sell_id;
    Price price;
    Quantity qty;
};
