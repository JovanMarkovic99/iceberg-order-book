
#include "../include/Order.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace jvn;

TEST(OrderTest, VirtualFunctionallity) {
  std::unique_ptr<Order> iceberg_order(new IcebergOrder(OrderType::BUY, 0, 0, 100, 30));
  iceberg_order->matchVolume(20);
  EXPECT_EQ(iceberg_order->getVolume(), 10);
  iceberg_order->matchVolume(10);
  EXPECT_EQ(iceberg_order->getVolume(), 30);
  iceberg_order->matchVolume(30);
  iceberg_order->matchVolume(30);
  EXPECT_EQ(iceberg_order->getVolume(), 10);
}