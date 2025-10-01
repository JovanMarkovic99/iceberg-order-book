
#include "../include/matching_engine.h"
#include "../include/printer.h"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <streambuf>

class MatchingEngineFixture: public ::testing::Test {
protected:
    void SetUp() override {
        org_cout = std::cout.rdbuf(buffer.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(org_cout);
    }

    std::stringstream buffer;
    std::streambuf *org_cout;
    MatchingEngine engine;
};

Order makeOrder(Order::Side side, OrderId id, Price price, Quantity qty, Quantity peak_qty = 0) {
    Quantity visible_qty = qty,
        initial_visible_qty = qty,
        hidden_qty = 0;
    if (peak_qty) {
        hidden_qty = visible_qty;
        initial_visible_qty = peak_qty;
        visible_qty = std::min(hidden_qty, initial_visible_qty);
        hidden_qty -= visible_qty;
    }
    return Order{
        .side = side,
        .id = id,
        .price = price,
        .visible_qty = visible_qty,
        .initial_visible_qty = initial_visible_qty,
        .hidden_qty = hidden_qty
    };
}

TEST_F(MatchingEngineFixture, AgressiveIcebergOrder) {
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 100322, 5103, 7500)));
    EXPECT_EQ(buffer.str(), "");

    Printer::print(engine);
    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|    100322|        7,500|  5,103|       |             |          |\n"
    "+-----------------------------------------------------------------+\n");
    buffer.str("");


    Printer::print(engine.process(makeOrder(Order::Side::SELL, 100345, 5103, 100000, 10000)));
    EXPECT_EQ(buffer.str(), "100322,100345,5103,7500\n");
    buffer.str("");

    Printer::print(engine);
    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |  5,103|       10,000|    100345|\n"
    "+-----------------------------------------------------------------+\n");
    buffer.str("");
}


TEST_F(MatchingEngineFixture, SimpleExample1) {
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 1, 100, 50, 10)));
    Printer::print(engine);
    Printer::print(engine.process(makeOrder(Order::Side::SELL, 2, 10, 100, 10)));
    Printer::print(engine);
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 3, 20, 100, 10)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|         1|           10|    100|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    "1,2,100,50\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |     10|           10|         2|\n"
    "+-----------------------------------------------------------------+\n"
    "3,2,10,50\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|         3|           10|     20|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
}


TEST_F(MatchingEngineFixture, SimpleExample2) {
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 1, 10, 10)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|         1|           10|     10|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(makeOrder(Order::Side::BUY, 2, 11, 10)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|         2|           10|     11|       |             |          |\n"
    "|         1|           10|     10|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(makeOrder(Order::Side::SELL, 3, 9, 20)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "2,3,11,10\n"
    "1,3,10,10\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "+-----------------------------------------------------------------+\n"
    );
}


TEST_F(MatchingEngineFixture, ComplexExample1) {
    Printer::print(engine.process(makeOrder(Order::Side::SELL, 1, 100, 50, 10)));
    Printer::print(engine);
    Printer::print(engine.process(makeOrder(Order::Side::SELL, 2, 100, 10)));
    Printer::print(engine);
    Printer::print(engine.process(makeOrder(Order::Side::SELL, 3, 101, 5)));
    Printer::print(engine);
    Printer::print(engine.process(makeOrder(Order::Side::SELL, 4, 99, 3)));
    Printer::print(engine);

    Printer::print(engine.process(makeOrder(Order::Side::BUY, 5, 105, 2)));
    Printer::print(engine);
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 6, 100, 31)));
    Printer::print(engine);
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 7, 100, 31)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |    100|           10|         1|\n"
    "+-----------------------------------------------------------------+\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |    100|           10|         1|\n"
    "|          |             |       |    100|           10|         2|\n"
    "+-----------------------------------------------------------------+\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |    100|           10|         1|\n"
    "|          |             |       |    100|           10|         2|\n"
    "|          |             |       |    101|            5|         3|\n"
    "+-----------------------------------------------------------------+\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |     99|            3|         4|\n"
    "|          |             |       |    100|           10|         1|\n"
    "|          |             |       |    100|           10|         2|\n"
    "|          |             |       |    101|            5|         3|\n"
    "+-----------------------------------------------------------------+\n"
    "5,4,99,2\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |     99|            1|         4|\n"
    "|          |             |       |    100|           10|         1|\n"
    "|          |             |       |    100|           10|         2|\n"
    "|          |             |       |    101|            5|         3|\n"
    "+-----------------------------------------------------------------+\n"
    "6,4,99,1\n"
    "6,1,100,20\n"
    "6,2,100,10\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |    100|           10|         1|\n"
    "|          |             |       |    101|            5|         3|\n"
    "+-----------------------------------------------------------------+\n"
    "7,1,100,30\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|         7|            1|    100|    101|            5|         3|\n"
    "+-----------------------------------------------------------------+\n"
    );
}


TEST_F(MatchingEngineFixture, ComplexExample2) {
    Printer::print(engine.process(
        makeOrder(Order::Side::BUY, 82025, 99, 50000)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82025|       50,000|     99|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(
        makeOrder(Order::Side::BUY, 82409, 98, 25500)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82025|       50,000|     99|       |             |          |\n"
    "|     82409|       25,500|     98|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(
        makeOrder(Order::Side::SELL, 81900, 101, 20000)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82025|       50,000|     99|    101|       20,000|     81900|\n"
    "|     82409|       25,500|     98|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(makeOrder(Order::Side::SELL, 82032, 100, 10000)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82025|       50,000|     99|    100|       10,000|     82032|\n"
    "|     82409|       25,500|     98|    101|       20,000|     81900|\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(makeOrder(Order::Side::SELL, 82257, 100, 7500)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82025|       50,000|     99|    100|       10,000|     82032|\n"
    "|     82409|       25,500|     98|    100|        7,500|     82257|\n"
    "|          |             |       |    101|       20,000|     81900|\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(makeOrder(Order::Side::BUY, 82500, 100, 100000, 10000)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "82500,82032,100,10000\n"
    "82500,82257,100,7500\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82500|       10,000|    100|    101|       20,000|     81900|\n"
    "|     82025|       50,000|     99|       |             |          |\n"
    "|     82409|       25,500|     98|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
}

TEST_F(MatchingEngineFixture, ComplexExample3) {
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 82532, 100, 82500, 10000)));
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 82025, 99, 50000)));
    Printer::print(engine.process(makeOrder(Order::Side::BUY, 82409, 98, 25500)));
    Printer::print(engine.process(makeOrder(Order::Side::SELL, 81900, 101, 20000)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82532|       10,000|    100|    101|       20,000|     81900|\n"
    "|     82025|       50,000|     99|       |             |          |\n"
    "|     82409|       25,500|     98|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(makeOrder(Order::Side::SELL, 82612, 100, 11000)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "82532,82612,100,11000\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82532|        9,000|    100|    101|       20,000|     81900|\n"
    "|     82025|       50,000|     99|       |             |          |\n"
    "|     82409|       25,500|     98|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    Printer::print(engine.process(makeOrder(Order::Side::BUY, 82800, 100, 50000, 20000)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82532|        9,000|    100|    101|       20,000|     81900|\n"
    "|     82800|       20,000|    100|       |             |          |\n"
    "|     82025|       50,000|     99|       |             |          |\n"
    "|     82409|       25,500|     98|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");


    Printer::print(engine.process(makeOrder(Order::Side::SELL, 83000, 100, 35000)));
    Printer::print(engine);

    EXPECT_EQ(buffer.str(),
    "82532,83000,100,15000\n"
    "82800,83000,100,20000\n"
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82532|        4,000|    100|    101|       20,000|     81900|\n"
    "|     82800|       20,000|    100|       |             |          |\n"
    "|     82025|       50,000|     99|       |             |          |\n"
    "|     82409|       25,500|     98|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");
}