#include "execution_engine.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include "order.h"

ExecutionEngine::ExecutionEngine()
    : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}

void ExecutionEngine::processOrdersInParallel(
    const std::vector<std::shared_ptr<Order>>& orders) {
  const int& num_threads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  auto process_chunk = [&](const int& start, const int& end) {
    for (int i = start; i < end; ++i) {
      processOrder(orders[i]);
    }
  };

  const int& chunk_size = orders.size() / num_threads;
  for (int i = 0; i < num_threads; ++i) {
    int start = i * chunk_size;
    int end = (i == num_threads - 1) ? orders.size() : start + chunk_size;
    threads.emplace_back(process_chunk, start, end);
  }

  for (auto& thread : threads) {
    thread.join();
  }
}

void ExecutionEngine::processOrder(const std::shared_ptr<Order>& order) {
  std::mutex lock;
  lock.lock();
  if (order->type == OrderType::MARKET) {
    executeMarketOrder(order);
  } else if (order->type == OrderType::LIMIT) {
    executeLimitOrder(order);
  } else if (order->type == OrderType::STOP) {
    addStopOrder(order);
  }
  checkStopOrders();
  lock.unlock();
}

void ExecutionEngine::addStopOrder(const std::shared_ptr<Order>& order) {
  std::cout << "Stop order added: " << order->id << " at price " << order->price
            << '\n';
  stopOrders.push_back(order);
}

void ExecutionEngine::checkStopOrders() {
  auto it = stopOrders.begin();
  while (it != stopOrders.end()) {
    const auto& order = *it;
    bool triggered = false;

    if (order->side == OrderSide::BUY && lastTradePrice >= order->price) {
      triggered = true;
    } else if (order->side == OrderSide::SELL &&
               lastTradePrice <= order->price) {
      triggered = true;
    }

    if (triggered) {
      std::cout << "Stop order triggered: " << order->id << '\n';
      order->type = OrderType::MARKET;
      executeMarketOrder(order);
      it = stopOrders.erase(it);
    } else {
      ++it;
    }
  }
}

void ExecutionEngine::updateLastTradePrice(double price) {
  lastTradePrice = price;
  checkStopOrders();
}

void ExecutionEngine::executeMarketOrder(const std::shared_ptr<Order>& order) {
  const auto& counterBook = (order->side == OrderSide::BUY)
                                ? orderBook.GetBidBook().getTopOfBook()
                                : orderBook.GetAskBook().getTopOfBook();

  if (counterBook.empty()) {
    std::cout << "No orders in the book to match against." << '\n';
    return;
  }

  int remainingQuantity = order->quantity;

  for (const auto& level : counterBook) {
    if (remainingQuantity <= 0) break;

    const int& executedQuantity = std::min(remainingQuantity, level.second);
    remainingQuantity -= executedQuantity;

    std::cout << "Executed " << executedQuantity << " at price " << level.first
              << '\n';
    updateLastTradePrice(level.first);
  }

  if (remainingQuantity > 0) {
    std::cout << "Market order partially filled. Remaining quantity: "
              << remainingQuantity << '\n';
  }
}

void ExecutionEngine::executeLimitOrder(const std::shared_ptr<Order>& order) {
  const auto& bidBook = orderBook.GetBidBook();
  const auto& askBook = orderBook.GetBidBook();

  if (bidBook.isEmpty() && askBook.isEmpty()) {
    std::cout << "Limit order added to the book: " << order->id << '\n';
    orderBook.addOrder(order);
    return;
  }

  const bool& canExecute =
      (order->side == OrderSide::BUY && order->price >= bidBook.GetPrice()) ||
      (order->side == OrderSide::SELL && order->price <= askBook.GetPrice());

  if (canExecute) {
    executeMarketOrder(order);
  } else {
    std::cout << "Limit order added to the book: " << order->id << '\n';
    orderBook.addOrder(order);
  }
}

std::shared_ptr<Order> ExecutionEngine::generateRandomOrder() {
  const OrderType& type =
      (rng() % 2 == 0) ? OrderType::MARKET : OrderType::LIMIT;
  const OrderSide& side = (rng() % 2 == 0) ? OrderSide::BUY : OrderSide::SELL;
  const double& price = 100.0 + (rng() % 1000) / 100.0;
  const int& quantity = 1 + (rng() % 100);

  return std::make_shared<Order>(nextOrderId++, type, side, price, quantity);
}

void ExecutionEngine::simulateTrading(int numOrders) {
  std::vector<std::shared_ptr<Order>> orders;
  for (int i = 0; i < numOrders; ++i) {
    orders.emplace_back(generateRandomOrder());
    std::cout << "Processing order: " << orders.back()->id << '\n';
  }
  processOrdersInParallel(orders);
  printOrderBookStatus();
}

void ExecutionEngine::printOrderBookStatus() {
  std::cout << "Order Book Status:" << '\n';
  std::cout << "Bids:" << '\n';
  for (const auto& level : orderBook.GetBidBook().getTopOfBook()) {
    std::cout << "  Price: " << level.first << ", Quantity: " << level.second
              << '\n';
  }
  std::cout << "Asks:" << '\n';
  for (const auto& level : orderBook.GetAskBook().getTopOfBook()) {
    std::cout << "  Price: " << level.first << ", Quantity: " << level.second
              << '\n';
  }
  std::cout << '\n';
}
