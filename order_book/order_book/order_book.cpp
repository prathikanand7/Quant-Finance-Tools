#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>

enum class OrderType { MARKET, LIMIT, STOP };
enum class OrderSide { BUY, SELL };

class Order {
 public:
  Order(int id, OrderType type, OrderSide side, double price, int quantity)
      : id(id), type(type), side(side), price(price), quantity(quantity) {}

  int id;
  OrderType type;
  OrderSide side;
  double price;
  int quantity;
};

class OrderBook {
 private:
  std::map<double, std::vector<std::shared_ptr<Order>>, std::greater<>> bids;
  std::map<double, std::vector<std::shared_ptr<Order>>> asks;

 public:
  void addOrder(const std::shared_ptr<Order>& order) {
    if (order->side == OrderSide::BUY) {
      bids[order->price].push_back(order);
    } else {
      asks[order->price].push_back(order);
    }
  }

  void removeOrder(const std::shared_ptr<Order>& order) {
    auto& orderList = (order->side == OrderSide::BUY) ? bids[order->price]
                                                      : asks[order->price];
    orderList.erase(std::remove_if(orderList.begin(), orderList.end(),
                                   [&](const std::shared_ptr<Order>& o) {
                                     return o->id == order->id;
                                   }),
                    orderList.end());
    // If the list is empty after removal, delete the price level
    if (orderList.empty()) {
      if (order->side == OrderSide::BUY) {
        bids.erase(order->price);
      } else {
        asks.erase(order->price);
      }
    }
  }

  std::vector<std::pair<double, int>> getTopOfBook(const OrderSide side,
                                                   const int levels = 5) {
    std::vector<std::pair<double, int>> result;
    if (side == OrderSide::BUY) {
      for (const auto& level : bids) {
        if (result.size() >= levels) break;
        int totalQuantity = 0;
        for (const auto& order : level.second) {
          totalQuantity += order->quantity;
        }
        result.emplace_back(level.first, totalQuantity);
      }
    } else {  // OrderSide::SELL
      for (const auto& level : asks) {
        if (result.size() >= levels) break;
        int totalQuantity = 0;
        for (const auto& order : level.second) {
          totalQuantity += order->quantity;
        }
        result.emplace_back(level.first, totalQuantity);
      }
    }
    return result;
  }

  bool isEmpty(const OrderSide side) const {
    return (side == OrderSide::BUY) ? bids.empty() : asks.empty();
  }
};

class ExecutionEngine {
 private:
  OrderBook orderBook;
  std::vector<std::shared_ptr<Order>> stopOrders;
  int nextOrderId = 1;
  std::mt19937 rng;
  double lastTradePrice = 100.0;  // Initialize with a starting price

 public:
  ExecutionEngine()
      : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}

  void processOrder(const std::shared_ptr<Order>& order) {
    if (order->type == OrderType::MARKET) {
      executeMarketOrder(order);
    } else if (order->type == OrderType::LIMIT) {
      executeLimitOrder(order);
    } else if (order->type == OrderType::STOP) {
      addStopOrder(order);
    }
    checkStopOrders();
  }

  void addStopOrder(const std::shared_ptr<Order>& order) {
    stopOrders.push_back(order);
    std::cout << "Stop order added: " << order->id << " at price "
              << order->price << '\n';
  }

  void checkStopOrders() {
    auto it = stopOrders.begin();
    while (it != stopOrders.end()) {
      const auto order = *it;
      bool triggered = false;

      if (order->side == OrderSide::BUY && lastTradePrice >= order->price) {
        triggered = true;
      } else if (order->side == OrderSide::SELL &&
                 lastTradePrice <= order->price) {
        triggered = true;
      }

      if (triggered) {
        std::cout << "Stop order triggered: " << order->id << '\n';
        order->type = OrderType::MARKET;  // Convert to market order
        executeMarketOrder(order);
        it = stopOrders.erase(it);
      } else {
        ++it;
      }
    }
  }

  void updateLastTradePrice(double price) {
    lastTradePrice = price;
    checkStopOrders();
  }

  void executeMarketOrder(const std::shared_ptr<Order>& order) {
    const auto counterBook = (order->side == OrderSide::BUY)
                                 ? orderBook.getTopOfBook(OrderSide::SELL)
                                 : orderBook.getTopOfBook(OrderSide::BUY);

    if (counterBook.empty()) {
      std::cout << "No orders in the book to match against." << '\n';
      return;
    }

    int remainingQuantity = order->quantity;

    for (const auto& level : counterBook) {
      if (remainingQuantity <= 0) break;

      const int executedQuantity = std::min(remainingQuantity, level.second);
      remainingQuantity -= executedQuantity;

      std::cout << "Executed " << executedQuantity << " at price "
                << level.first << '\n';
      updateLastTradePrice(level.first);
    }

    if (remainingQuantity > 0) {
      std::cout << "Market order partially filled. Remaining quantity: "
                << remainingQuantity << '\n';
    }
  }

  void executeLimitOrder(const std::shared_ptr<Order>& order) {
    const auto counterBook = (order->side == OrderSide::BUY)
                                 ? orderBook.getTopOfBook(OrderSide::SELL)
                                 : orderBook.getTopOfBook(OrderSide::BUY);

    if (counterBook.empty()) {
      orderBook.addOrder(order);
      std::cout << "Limit order added to the book: " << order->id << '\n';
      return;
    }

    const bool canExecute = (order->side == OrderSide::BUY &&
                             order->price >= counterBook[0].first) ||
                            (order->side == OrderSide::SELL &&
                             order->price <= counterBook[0].first);

    if (canExecute) {
      executeMarketOrder(order);
    } else {
      orderBook.addOrder(order);
      std::cout << "Limit order added to the book: " << order->id << '\n';
    }
  }

  std::shared_ptr<Order> generateRandomOrder() {
    OrderType type = (rng() % 2 == 0) ? OrderType::MARKET : OrderType::LIMIT;
    OrderSide side = (rng() % 2 == 0) ? OrderSide::BUY : OrderSide::SELL;
    double price = 100.0 + (rng() % 1000) / 100.0;
    int quantity = 1 + (rng() % 100);

    return std::make_shared<Order>(nextOrderId++, type, side, price, quantity);
  }

  void simulateTrading(int numOrders) {
    for (int i = 0; i < numOrders; ++i) {
      const auto order = generateRandomOrder();
      std::cout << "Processing order: " << order->id << '\n';
      processOrder(order);
      printOrderBookStatus();
    }
  }

  void printOrderBookStatus() {
    std::cout << "Order Book Status:" << '\n';
    std::cout << "Bids:" << '\n';
    for (const auto& level : orderBook.getTopOfBook(OrderSide::BUY)) {
      std::cout << "  Price: " << level.first << ", Quantity: " << level.second
                << '\n';
    }
    std::cout << "Asks:" << '\n';
    for (const auto& level : orderBook.getTopOfBook(OrderSide::SELL)) {
      std::cout << "  Price: " << level.first << ", Quantity: " << level.second
                << '\n';
    }
    std::cout << '\n';
  }
};

int main() {
  ExecutionEngine engine;
  const auto start_time = std::chrono::high_resolution_clock::now();
  engine.simulateTrading(10000);
  const auto end_time = std::chrono::high_resolution_clock::now();
  std::cout << "Simulation took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                     start_time)
                   .count()
            << " ms" << '\n';
  return 0;
}
