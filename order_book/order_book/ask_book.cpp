#include "ask_book.h"

#include <algorithm>

void AskBook::addOrder(const std::shared_ptr<Order>& order) {
  const auto& it = std::lower_bound(prices.begin(), prices.end(), order->price);
  const size_t& index = std::distance(prices.begin(), it);
  if (it != prices.end() && *it == order->price) {
    quantities[index] += order->quantity;
  } else {
    prices.insert(it, order->price);
    quantities.insert(quantities.begin() + index, order->quantity);
  }
}

void AskBook::removeOrder(double price, int quantity) {
  const auto& it = std::find(prices.begin(), prices.end(), price);
  if (it != prices.end()) {
    const size_t& index = std::distance(prices.begin(), it);
    quantities[index] -= quantity;
    if (quantities[index] <= 0) {
      prices.erase(it);
      quantities.erase(quantities.begin() + index);
    }
  }
}

std::vector<std::pair<double, int>> AskBook::getTopOfBook(int levels) const {
  std::vector<std::pair<double, int>> result;
  for (size_t i = 0; i < prices.size() && result.size() < levels; ++i) {
    result.emplace_back(prices[i], quantities[i]);
  }
  return result;
}

bool AskBook::isEmpty() const { return prices.empty(); }

double AskBook::GetPrice() const { return prices[0]; }
