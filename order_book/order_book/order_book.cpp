#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <thread>
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
  std::vector<double> bidPrices;
  std::vector<double> bidQuantities;

  std::vector<double> askPrices;
  std::vector<double> askQuantities;

 public:
  void addOrder(const std::shared_ptr<Order>& order) {
    if (order->side == OrderSide::BUY) {
      addBid(order->price, order->quantity);
    } else {
      addAsk(order->price, order->quantity);
    }
  }

  void removeOrder(const std::shared_ptr<Order>& order) {
    if (order->side == OrderSide::BUY) {
      removeBid(order->price, order->quantity);
    } else {
      removeAsk(order->price, order->quantity);
    }
  }

  std::vector<std::pair<double, int>> getTopOfBook(const OrderSide side,
                                                   const int levels = 5) const {
    std::vector<std::pair<double, int>> result;
    if (side == OrderSide::BUY) {
      for (size_t i = 0; i < bidPrices.size() && result.size() < levels; ++i) {
        result.emplace_back(bidPrices[i], bidQuantities[i]);
      }
    } else {
      for (size_t i = 0; i < askPrices.size() && result.size() < levels; ++i) {
        result.emplace_back(askPrices[i], askQuantities[i]);
      }
    }
    return result;
  }

 private:
  void addBid(double price, int quantity) {
    const auto it = std::lower_bound(bidPrices.begin(), bidPrices.end(), price,
                                     std::greater<>());
    const size_t index = std::distance(bidPrices.begin(), it);
    if (it != bidPrices.end() && *it == price) {
      bidQuantities[index] += quantity;
    } else {
      bidPrices.insert(it, price);
      bidQuantities.insert(bidQuantities.begin() + index, quantity);
    }
  }

  void addAsk(double price, int quantity) {
    const auto it = std::lower_bound(askPrices.begin(), askPrices.end(), price);
    const size_t index = std::distance(askPrices.begin(), it);
    if (it != askPrices.end() && *it == price) {
      askQuantities[index] += quantity;
    } else {
      askPrices.insert(it, price);
      askQuantities.insert(askQuantities.begin() + index, quantity);
    }
  }

  void removeBid(double price, int quantity) {
    const auto it = std::find(bidPrices.begin(), bidPrices.end(), price);
    if (it != bidPrices.end()) {
      const size_t index = std::distance(bidPrices.begin(), it);
      bidQuantities[index] -= quantity;
      if (bidQuantities[index] <= 0) {
        bidPrices.erase(it);
        bidQuantities.erase(bidQuantities.begin() + index);
      }
    }
  }

  void removeAsk(double price, int quantity) {
    const auto it = std::find(askPrices.begin(), askPrices.end(), price);
    if (it != askPrices.end()) {
      const size_t index = std::distance(askPrices.begin(), it);
      askQuantities[index] -= quantity;
      if (askQuantities[index] <= 0) {
        askPrices.erase(it);
        askQuantities.erase(askQuantities.begin() + index);
      }
    }
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

  void processOrdersInParallel(
      const std::vector<std::shared_ptr<Order>>& orders) {
    const int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    auto process_chunk = [&](const int start, const int end) {
      for (int i = start; i < end; ++i) {
        processOrder(orders[i]);
      }
    };

    const int chunk_size = orders.size() / num_threads;
    for (int i = 0; i < num_threads; ++i) {
      int start = i * chunk_size;
      int end = (i == num_threads - 1) ? orders.size() : start + chunk_size;
      threads.emplace_back(process_chunk, start, end);
    }

    for (auto& thread : threads) {
      thread.join();
    }
  }

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
    std::cout << "Stop order added: " << order->id << " at price "
              << order->price << '\n';
    stopOrders.push_back(order);
  }

  void checkStopOrders() {
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
      std::cout << "Limit order added to the book: " << order->id << '\n';
      orderBook.addOrder(order);
      return;
    }

    const bool canExecute = (order->side == OrderSide::BUY &&
                             order->price >= counterBook[0].first) ||
                            (order->side == OrderSide::SELL &&
                             order->price <= counterBook[0].first);

    if (canExecute) {
      executeMarketOrder(order);
    } else {
      std::cout << "Limit order added to the book: " << order->id << '\n';
      orderBook.addOrder(order);
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
    std::vector<std::shared_ptr<Order>> orders;
    for (int i = 0; i < numOrders; ++i) {
      orders.emplace_back(generateRandomOrder());
      std::cout << "Processing order: " << orders.back()->id << '\n';
    }
    processOrdersInParallel(orders);
    printOrderBookStatus();
  }

  void printOrderBookStatus() const {
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
