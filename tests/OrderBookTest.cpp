
#include "../include/OrderBook.hpp"
#include "../include/OrderBookPrinter.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <streambuf>

using namespace jvn;

class OrderBookFixture: public ::testing::Test {
protected:
    void SetUp() override {
        org_cout = std::cout.rdbuf(buffer.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(org_cout);
    }

    std::stringstream buffer;
    std::streambuf *org_cout;
    OrderBook book;
};

std::ostream& operator<<(std::ostream& os, const OrderBook::Match& match) {
    os << match.buy_id << ',' 
        << match.sell_id << ',' 
        << match.limit << ',' 
        << match.quantity << '\n';
    return os;
}

void printMatches(const std::vector<OrderBook::Match>& matches) {
    for (const auto& match: matches)
        std::cout << match;
}

TEST_F(OrderBookFixture, AgressiveIcebergOrder) {
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::BUY, 100322, 5103, 7500))));
    EXPECT_EQ(buffer.str(), "");
    
    OrderBookPrinter::print(book);
    EXPECT_EQ(buffer.str(), 
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|    100322|        7,500|  5,103|       |             |          |\n"
    "+-----------------------------------------------------------------+\n");
    buffer.str("");

    
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new IcebergOrder(OrderType::SELL, 100345, 5103, 100000, 10000))));
    EXPECT_EQ(buffer.str(), "100322,100345,5103,7500\n");
    buffer.str("");

    OrderBookPrinter::print(book);
    EXPECT_EQ(buffer.str(), 
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|          |             |       |  5,103|       10,000|    100345|\n"
    "+-----------------------------------------------------------------+\n");
    buffer.str("");
}


TEST_F(OrderBookFixture, SimpleExample1) {
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new IcebergOrder(OrderType::BUY, 1, 100, 50, 10))));
    OrderBookPrinter::print(book);
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new IcebergOrder(OrderType::SELL, 2, 10, 100, 10))));
    OrderBookPrinter::print(book);
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new IcebergOrder(OrderType::BUY, 3, 20, 100, 10))));
    OrderBookPrinter::print(book);

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


TEST_F(OrderBookFixture, SimpleExample2) {
    printMatches(book.processOrder(std::unique_ptr<Order>(new Order(OrderType::BUY, 1, 10, 10))));
    OrderBookPrinter::print(book);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|         1|           10|     10|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    printMatches(book.processOrder(std::unique_ptr<Order>(new Order(OrderType::BUY, 2, 11, 10))));
    OrderBookPrinter::print(book);

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

    printMatches(book.processOrder(std::unique_ptr<Order>(new Order(OrderType::SELL, 3, 9, 20))));
    OrderBookPrinter::print(book);

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


TEST_F(OrderBookFixture, ComplexExample1) {
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new IcebergOrder(OrderType::SELL, 1, 100, 50, 10))));
    OrderBookPrinter::print(book);
    printMatches(book.processOrder(std::unique_ptr<Order>(new Order(OrderType::SELL, 2, 100, 10))));
    OrderBookPrinter::print(book);
    printMatches(book.processOrder(std::unique_ptr<Order>(new Order(OrderType::SELL, 3, 101, 5))));
    OrderBookPrinter::print(book);
    printMatches(book.processOrder(std::unique_ptr<Order>(new Order(OrderType::SELL, 4, 99, 3))));
    OrderBookPrinter::print(book);

    printMatches(book.processOrder(std::unique_ptr<Order>(new Order(OrderType::BUY, 5, 105, 2))));
    OrderBookPrinter::print(book);
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::BUY, 6, 100, 31))));
    OrderBookPrinter::print(book);
    printMatches(book.processOrder(std::unique_ptr<Order>(new Order(OrderType::BUY, 7, 100, 31))));
    OrderBookPrinter::print(book);

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


TEST_F(OrderBookFixture, ComplexExample2) {
    printMatches(book.processOrder(
        std::unique_ptr<Order>(new Order(OrderType::BUY, 82025, 99, 50000))));
    OrderBookPrinter::print(book);

    EXPECT_EQ(buffer.str(),
    "+-----------------------------------------------------------------+\n"
    "| BUY                            | SELL                           |\n"
    "| Id       | Volume      | Price | Price | Volume      | Id       |\n"
    "+----------+-------------+-------+-------+-------------+----------+\n"
    "|     82025|       50,000|     99|       |             |          |\n"
    "+-----------------------------------------------------------------+\n"
    );
    buffer.str("");

    printMatches(book.processOrder(
        std::unique_ptr<Order>(new Order(OrderType::BUY, 82409, 98, 25500))));
    OrderBookPrinter::print(book);

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

    printMatches(book.processOrder(
        std::unique_ptr<Order>(new Order(OrderType::SELL, 81900, 101, 20000))));
    OrderBookPrinter::print(book);

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

    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::SELL, 82032, 100, 10000))));
    OrderBookPrinter::print(book);

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

    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::SELL, 82257, 100, 7500))));
    OrderBookPrinter::print(book);

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

    printMatches(book.processOrder(std::unique_ptr<Order>(
        new IcebergOrder(OrderType::BUY, 82500, 100, 100000, 10000))));
    OrderBookPrinter::print(book);

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

TEST_F(OrderBookFixture, ComplexExample3) {
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new IcebergOrder(OrderType::BUY, 82532, 100, 82500, 10000))));
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::BUY, 82025, 99, 50000))));
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::BUY, 82409, 98, 25500))));
    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::SELL, 81900, 101, 20000))));
    OrderBookPrinter::print(book);

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

    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::SELL, 82612, 100, 11000))));
    OrderBookPrinter::print(book);

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

    printMatches(book.processOrder(std::unique_ptr<Order>(
        new IcebergOrder(OrderType::BUY, 82800, 100, 50000, 20000))));
    OrderBookPrinter::print(book);

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


    printMatches(book.processOrder(std::unique_ptr<Order>(
        new Order(OrderType::SELL, 83000, 100, 35000))));
    OrderBookPrinter::print(book);

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