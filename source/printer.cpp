#include <iomanip>

#include "../include/printer.h"

void Printer::print(const Trade& trade, std::ostream& os) {
    os << trade.buy_id << ','
        << trade.sell_id << ','
        << trade.price << ','
        << trade.qty << '\n';
}

void Printer::print(const std::vector<Trade>& trades, std::ostream& os) {
    for (const auto& trade: trades)
        print(trade, os);
}

void Printer::print(const MatchingEngine& engine, std::ostream& os) {
    printHeader(os);

    os << std::right;

    // +----------+-------------+-------+-------+-------------+----------+
    for (size_t section_idx: SECTION_INDICES) {
        const SectionInfo& section_info = TABLE_SECTIONS[section_idx];
        printTextSection("", section_info.width, HORIZONTAL_EDGE_CHAR, os, CORNER_CHAR);
    }
    os << CORNER_CHAR << '\n';

    auto buy_iter = engine.buyBegin();
    auto sell_iter = engine.sellBegin();
    while (buy_iter != engine.buyEnd() || sell_iter != engine.sellEnd()) {
        printOrderSection(
            buy_iter != engine.buyEnd() ? &(*buy_iter++) : nullptr, Order::Side::BUY, os);
        printOrderSection(
            sell_iter != engine.sellEnd() ? &(*sell_iter++) : nullptr, Order::Side::SELL, os);
        os << VERTICAL_EDGE_CHAR << '\n';
    }

    // +-----------------------------------------------------------------+
    printTextSection("", TOTAL_COLUMN_WIDTH, HORIZONTAL_EDGE_CHAR, os,
        CORNER_CHAR, CORNER_CHAR, true);
}

void Printer::printHeader(std::ostream& os) {
    os << std::left;

    // +-----------------------------------------------------------------+
    printTextSection("", TOTAL_COLUMN_WIDTH, HORIZONTAL_EDGE_CHAR, os,
        CORNER_CHAR, CORNER_CHAR, true);

    // | BUY                            | SELL                           |
    printTextSection(" BUY ", HALF_COLUMN_WIDTH, SEPARATOR_CHAR, os, VERTICAL_EDGE_CHAR);
    printTextSection(" SELL ", HALF_COLUMN_WIDTH, SEPARATOR_CHAR, os,
        VERTICAL_EDGE_CHAR, VERTICAL_EDGE_CHAR, true);

    // | Id       | Volume      | Price | Price | Volume      | Id       |
    for (size_t section_idx: SECTION_INDICES) {
        const SectionInfo& section_info = TABLE_SECTIONS[section_idx];
        printTextSection(section_info.text, section_info.width, SEPARATOR_CHAR, os,
            VERTICAL_EDGE_CHAR);
    }
    os << VERTICAL_EDGE_CHAR << '\n';
}

void Printer::printTextSection(
    std::string_view text, size_t width, char fill_char, std::ostream& os,
    char prefix, char suffix, bool end_line) {
    if (prefix != '\0')
        os << prefix;

    os << std::setfill(fill_char) << std::setw(static_cast<int>(width)) << text;

    if (suffix != '\0')
        os << suffix;

    if (end_line)
        os << '\n';
}

void Printer::printOrderSection(const Order* order, Order::Side side, std::ostream& os) {
    assert((order ? order->side : side) == side);

    auto [start_idx, end_idx] = (side == Order::Side::BUY)
        ? std::pair{size_t{0}, SECTION_INDICES.size() / 2}
        : std::pair{SECTION_INDICES.size() / 2, SECTION_INDICES.size()};
    for (size_t i = start_idx; i < end_idx; ++i) {
        const SectionInfo& section_info = TABLE_SECTIONS[SECTION_INDICES[i]];
        auto field =
            order ? intToString(section_info.getter(*order), section_info.format_number) : "";
        printTextSection(field, section_info.width, SEPARATOR_CHAR, os, VERTICAL_EDGE_CHAR);
    }
}

std::string Printer::intToString(int64_t num, bool format) {
    auto result = std::to_string(num);
    if (!format)
        return result;

    constexpr size_t GROUP_SIZE = 3;
    constexpr char GROUP_SEPARATOR = ',';

    bool negative = result[0] == '-';
    std::string formated_result;
    formated_result.reserve(result.size() + result.size() / GROUP_SIZE);
    for (size_t idx = 0; idx < result.size(); ++idx) {
        if (idx > static_cast<size_t>(negative) && (result.size() - idx) % GROUP_SIZE == 0)
            formated_result.push_back(GROUP_SEPARATOR);
        formated_result.push_back(result[idx]);
    }
    return formated_result;
}