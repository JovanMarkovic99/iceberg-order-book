
#include "../include/Limit.hpp"

#include <gtest/gtest.h>
#include <vector>

using namespace jvn;

constexpr size_t NUM_ORDERS = 10;

std::unique_ptr<Order> createIdxOrder(size_t idx) {
    return std::make_unique<Order>(idx % 2 ? OrderType::BUY : OrderType::SELL, 
                                2 * idx, 
                                0, 
                                3 * idx);
}

TEST(LimitTest, FullFunctionality) {
    Limit limit(0);
    EXPECT_TRUE(limit.isEmpty());

    Order* orders[NUM_ORDERS];
    for (size_t idx = 0; idx < NUM_ORDERS; ++idx) {
        orders[idx] = createIdxOrder(idx).release();
        limit.pushBack(std::unique_ptr<Order>(orders[idx]));
    }

    size_t idx = 0;
    for (const auto& ptr: limit)
        EXPECT_EQ(orders[idx++], ptr.get());

    for (size_t idx = 0; idx < NUM_ORDERS; ++idx)
        limit.pushBack(limit.popFront());

    idx = 0;
    for (const auto& ptr: limit)
        EXPECT_EQ(orders[idx++], ptr.get());
}