#pragma once

#include <memory>
#include <vector>

#include "Order.h"

class BidBook {
 private:
  std::vector<double> prices;
  std::vector<int> quantities;

 public:
  void addOrder(const std::shared_ptr<Order>& order);
  void removeOrder(double price, int quantity);
  std::vector<std::pair<double, int>> getTopOfBook(int levels = 5) const;
  bool isEmpty() const;
  double GetPrice() const;
};
