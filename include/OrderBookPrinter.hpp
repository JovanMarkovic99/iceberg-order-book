#pragma once

#include "OrderBook.hpp"

#include <functional>
#include <string_view>
#include <array>
#include <locale>
#include <ostream>

namespace jvn 
{
    class OrderBookPrinter {
    public:
        static void print(const OrderBook& order_book);
    private:
        static constexpr size_t ID_COLUMN_WIDTH = 10u;
        static constexpr size_t VOLUME_COLUMN_WIDTH = 13u;
        static constexpr size_t PRICE_COLUMN_WIDTH = 7u;

        static constexpr size_t HALF_COLUMN_WIDTH = 32u;
        static constexpr size_t TOTAL_COLUMN_WIDTH = 65u;

        static constexpr char CORNER_CHAR = '+';
        static constexpr char HORIZONTAL_EDGE_CHAR = '-';
        static constexpr char VERTICAL_EDGE_CHAR = '|';
        static constexpr char SEPARATOR_CHAR = ' ';

        using order_member_getter = int(*)(const Order*);
        struct SectionInfo {
            std::string_view text;
            size_t width;
            order_member_getter getter;
            bool format_number;
        };
        static constexpr std::array<SectionInfo, 3u> TABLE_SECTIONS = {{
            {
                " Id ", 
                ID_COLUMN_WIDTH, 
                [](const Order* order){ return static_cast<int>(order->getId()); }, 
                false
            },
            {
                " Volume ", 
                VOLUME_COLUMN_WIDTH, 
                [](const Order* order){ return static_cast<int>(order->getVolume()); }, 
                true
            },
            {
                " Price ", 
                PRICE_COLUMN_WIDTH, 
                [](const Order* order){ return static_cast<int>(order->getLimit()); }, 
                true}
        }};
        static constexpr std::array<size_t, 6u> SECTION_INDICES = {0, 1, 2, 2, 1, 0};

        static void printTextSection(std::string_view text, 
                                    int width, 
                                    char fill_char, 
                                    char prefix = '\0', 
                                    char suffix = '\0', 
                                    bool end_line = false);

        struct comma_three_digit_sep: public std::numpunct<char> {
            virtual std::string do_grouping() const { return "\003"; }
            virtual char do_thousands_sep() const { return ','; }
        };

        [[nodiscard]] static std::string intToText(int num, bool format);

        static void printHeader();

        static void printOrders(const OrderBook& order_book);
        template <OrderType OrderTy>
        static void printOrder(const Order* order);
    };
}