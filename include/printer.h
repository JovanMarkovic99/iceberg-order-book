#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <cstdint>

#include "matching_engine.h"

class Printer {
public:
    static void print(const Trade& trade, std::ostream& os = std::cout);

    static void print(const std::vector<Trade>& trades, std::ostream& os = std::cout);

    static void print(const MatchingEngine& engine, std::ostream& os = std::cout);

private:
    static constexpr size_t ID_COLUMN_WIDTH = 10;
    static constexpr size_t VOLUME_COLUMN_WIDTH = 13;
    static constexpr size_t PRICE_COLUMN_WIDTH = 7;

    static constexpr size_t HALF_COLUMN_WIDTH = 32;
    static constexpr size_t TOTAL_COLUMN_WIDTH = 65;

    static constexpr char CORNER_CHAR = '+';
    static constexpr char HORIZONTAL_EDGE_CHAR = '-';
    static constexpr char VERTICAL_EDGE_CHAR = '|';
    static constexpr char SEPARATOR_CHAR = ' ';

    using OrderMemberGetter = int64_t(*)(const Order&);
    struct SectionInfo {
        std::string_view text;
        size_t width;
        OrderMemberGetter getter;
        bool format_number;
    };
    static constexpr std::array<SectionInfo, 3> TABLE_SECTIONS{
        SectionInfo{
            .text = " Id ",
            .width = ID_COLUMN_WIDTH,
            .getter = [](const Order& order) -> int64_t { return order.id; },
            .format_number = false
        },
        SectionInfo{
            .text = " Volume ",
            .width = VOLUME_COLUMN_WIDTH,
            .getter = [](const Order& order) -> int64_t { return order.visible_qty; },
            .format_number = true
        },
        SectionInfo{
            .text = " Price ",
            .width = PRICE_COLUMN_WIDTH,
            .getter = [](const Order& order) -> int64_t { return order.price; },
            .format_number = true
        }
    };
    // Order of columns: Id, Volume, Price | Price, Volume, Id
    static constexpr std::array<size_t, 6> SECTION_INDICES = {0, 1, 2, 2, 1, 0};

    static void printHeader(std::ostream &os);

    static void printTextSection(
        std::string_view text, size_t width, char fill_char, std::ostream& os,
        char prefix = '\0', char suffix = '\0', bool end_line = false);

    static void printOrderSection(const Order* order, Order::Side side, std::ostream& os);

    [[nodiscard]] static std::string intToString(int64_t num, bool format);

};
