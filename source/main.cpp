
#include "../include/OrderBookPrinter.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace jvn;

std::vector<std::string> delimString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, ','))
        tokens.push_back(token);

    return tokens;
}

std::unique_ptr<Order> generateOrder() {
    std::string input;
    std::getline(std::cin, input);

    input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end());

    if (!input.size() || (input[0] != 'B' && input[0] != 'S'))
        return nullptr;

    auto tokens = delimString(input, ',');
    OrderType order_type = (tokens[0] == "B") ? OrderType::BUY : OrderType::SELL;
    Order::id_type id = std::stoi(tokens[1]);
    Order::limit_type limit = static_cast<Order::limit_type>(std::stoi(tokens[2]));
    Order::quantity_type quantity = std::stoi(tokens[3]);

    if (tokens.size() == 4)
        return std::make_unique<Order>(order_type, id, limit, quantity);

    Order::quantity_type peak_size = std::stoi(tokens[4]);
    
    return std::make_unique<IcebergOrder>(order_type, id, limit, quantity, peak_size);
}

std::ostream& operator<<(std::ostream& os, const OrderBook::Match& match) {
    os << match.buy_id << ',' 
        << match.sell_id << ',' 
        << match.limit << ',' 
        << match.quantity << '\n';
    return os;
}

int main() {
    OrderBook book;
    for (std::unique_ptr<Order> order = nullptr; ;order = generateOrder())
        if (order) {
            for (const auto& match: book.processOrder(std::move(order)))
                std::cout << match;
            OrderBookPrinter::print(book);
        }

    return 0;
}
