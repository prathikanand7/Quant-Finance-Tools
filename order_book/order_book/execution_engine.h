#pragma once

#include <memory>
#include <random>
#include <vector>

#include "order.h"
#include "order_book.h"

class ExecutionEngine {
 private:
  OrderBook orderBook;
  std::vector<std::shared_ptr<Order>> stopOrders;
  int nextOrderId = 1;
  std::mt19937 rng;
  double lastTradePrice = 100.0;

 public:
  ExecutionEngine();
  void processOrdersInParallel(
      const std::vector<std::shared_ptr<Order>>& orders);
  void processOrder(const std::shared_ptr<Order>& order);
  void addStopOrder(const std::shared_ptr<Order>& order);
  void checkStopOrders();
  void updateLastTradePrice(double price);
  void executeMarketOrder(const std::shared_ptr<Order>& order);
  void executeLimitOrder(const std::shared_ptr<Order>& order);
  std::shared_ptr<Order> generateRandomOrder();
  void simulateTrading(int numOrders);
  void printOrderBookStatus();
};
