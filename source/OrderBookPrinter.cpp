
#include "../include/OrderBookPrinter.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace jvn 
{
    void OrderBookPrinter::print(const OrderBook& order_book) {
        printHeader();
        printOrders(order_book);
    }

    void OrderBookPrinter::printHeader() {
        std::cout << std::left;

        // +-----------------------------------------------------------------+
        printTextSection("", 
                        TOTAL_COLUMN_WIDTH, 
                        HORIZONTAL_EDGE_CHAR, 
                        CORNER_CHAR, 
                        CORNER_CHAR, 
                        true);


        // | BUY                            | SELL                           |
        printTextSection(" BUY ", HALF_COLUMN_WIDTH, SEPARATOR_CHAR, VERTICAL_EDGE_CHAR);
        printTextSection(" SELL ", 
                        HALF_COLUMN_WIDTH, 
                        SEPARATOR_CHAR, 
                        VERTICAL_EDGE_CHAR, 
                        VERTICAL_EDGE_CHAR, 
                        true);


        // | Id       | Volume      | Price | Price | Volume      | Id       |
        for (size_t section_idx: SECTION_INDICES) {
            const SectionInfo& section_info = TABLE_SECTIONS[section_idx];
            printTextSection(section_info.text, 
                            section_info.width, 
                            SEPARATOR_CHAR, 
                            VERTICAL_EDGE_CHAR);
        }
        std::cout << VERTICAL_EDGE_CHAR << '\n';
    }

    void OrderBookPrinter::printOrders(const OrderBook& order_book) {
        std::cout << std::right;

        // +----------+-------------+-------+-------+-------------+----------+
        for (size_t section_idx: SECTION_INDICES) {
            const SectionInfo& section_info = TABLE_SECTIONS[section_idx];
            printTextSection("", section_info.width, HORIZONTAL_EDGE_CHAR, CORNER_CHAR);
        }
        std::cout << CORNER_CHAR << '\n';


        auto buy_map_iter = order_book.buyBegin();
        auto sell_map_iter = order_book.sellBegin();

        Limit::const_iterator buy_limit_iter;
        if (buy_map_iter != order_book.buyEnd()) 
            buy_limit_iter = buy_map_iter->second.begin();
        Limit::const_iterator sell_limit_iter;
        if (sell_map_iter != order_book.sellEnd()) 
            sell_limit_iter = sell_map_iter->second.begin();

        const auto advance_iters = [](auto& limit_iter, auto& map_iter, const auto& map_iter_end) {
            if (++limit_iter == map_iter->second.end())
                if (++map_iter != map_iter_end)
                    limit_iter = map_iter->second.begin();
        };

        while (buy_map_iter != order_book.buyEnd() || sell_map_iter != order_book.sellEnd()) {
            if (buy_map_iter != order_book.buyEnd()) {
                printOrder<OrderType::BUY>(buy_limit_iter->get());
                advance_iters(buy_limit_iter, buy_map_iter, order_book.buyEnd());
            } else 
                printOrder<OrderType::BUY>(nullptr);

            if (sell_map_iter != order_book.sellEnd()) {
                printOrder<OrderType::SELL>(sell_limit_iter->get());
                advance_iters(sell_limit_iter, sell_map_iter, order_book.sellEnd());
            } else
                printOrder<OrderType::SELL>(nullptr);

            std::cout << VERTICAL_EDGE_CHAR << '\n';
        }


        // +-----------------------------------------------------------------+
        printTextSection("", 
                        TOTAL_COLUMN_WIDTH, 
                        HORIZONTAL_EDGE_CHAR, 
                        CORNER_CHAR, 
                        CORNER_CHAR, 
                        true);
    }

    template <OrderType OrderTy>
    void OrderBookPrinter::printOrder(const Order* order) {
        constexpr auto section_idx_pairs = (OrderTy == OrderType::BUY) 
                                            ? std::pair<size_t, size_t>(0u, 
                                                                        SECTION_INDICES.size() / 2)
                                            : std::pair<size_t, size_t>(SECTION_INDICES.size() / 2,
                                                                        SECTION_INDICES.size());
        constexpr auto start_idx = section_idx_pairs.first;
        constexpr auto end_idx = section_idx_pairs.second;

        for (size_t i = start_idx; i < end_idx; ++i) {
            const SectionInfo& section_info = TABLE_SECTIONS[SECTION_INDICES[i]];
            printTextSection(order 
                                ? intToText(section_info.getter(order), section_info.format_number)
                                : "",
                            section_info.width,
                            SEPARATOR_CHAR,
                            VERTICAL_EDGE_CHAR);
        }
    }

    // There might be a better way to achive this
    std::string OrderBookPrinter::intToText(int num, bool format) {
        std::stringstream ss;
        if (format)
            ss.imbue(std::locale(ss.getloc(), new comma_three_digit_sep()));
        ss << num;
        return ss.str();
    }

    void OrderBookPrinter::printTextSection(std::string_view text, 
                                            int width, 
                                            char fill_char, 
                                            char prefix, 
                                            char suffix, 
                                            bool end_line) {
        if (prefix != '\0')
            std::cout << prefix;

        std::cout << std::setfill(fill_char) << std::setw(width) << text;

        if (suffix != '\0')
            std::cout << suffix;

        if (end_line)
            std::cout << '\n';
    }
}