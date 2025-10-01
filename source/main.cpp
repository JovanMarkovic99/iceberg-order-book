#include <optional>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

#include "../include/printer.h"

std::vector<std::string> delimString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

std::optional<Order> getOrder() {
    std::string input;
    std::getline(std::cin, input);
    input.erase(std::remove_if(std::begin(input), std::end(input), ::isspace), std::end(input));

    if (!input.size() || (input[0] != 'B' && input[0] != 'S'))
        return std::nullopt;

    auto tokens = delimString(input, ',');
    Order::Side side = (tokens[0] == "B") ? Order::Side::BUY : Order::Side::SELL;
    OrderId id = std::stoi(tokens[1]);
    Price price = static_cast<Price>(std::stoi(tokens[2]));
    Quantity qty = std::stoi(tokens[3]),
        visible_qty = qty,
        initial_visible_qty = qty,
        hidden_qty = 0;

    if (tokens.size() > 4) {
        initial_visible_qty = std::stoi(tokens[4]);
        hidden_qty = visible_qty;
        visible_qty = std::min(initial_visible_qty, hidden_qty);
        hidden_qty -= visible_qty;
    }

    return Order{
        .side = side,
        .type = Order::Type::LIMIT,
        .id = id,
        .price = price,
        .visible_qty = visible_qty,
        .initial_visible_qty = initial_visible_qty,
        .hidden_qty = hidden_qty
    };
}

int main() {
    MatchingEngine engine;
    for (auto order = getOrder(); ; order = getOrder())
        if (order) {
            for (const auto &match : engine.process(*order))
                Printer::print(match);
            Printer::print(engine);
        }
}
