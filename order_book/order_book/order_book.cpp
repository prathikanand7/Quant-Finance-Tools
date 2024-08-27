#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
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

class BidBook {
 private:
  std::vector<double> prices;
  std::vector<int> quantities;

 public:
  void addOrder(const std::shared_ptr<Order>& order) {
    const auto& it = std::lower_bound(prices.begin(), prices.end(),
                                      order->price, std::greater<>());
    const size_t& index = std::distance(prices.begin(), it);
    if (it != prices.end() && *it == order->price) {
      quantities[index] += order->quantity;
    } else {
      prices.insert(it, order->price);
      quantities.insert(quantities.begin() + index, order->quantity);
    }
  }

  void removeOrder(double price, int quantity) {
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

  std::vector<std::pair<double, int>> getTopOfBook(int levels = 5) const {
    std::vector<std::pair<double, int>> result;
    for (size_t i = 0; i < prices.size() && result.size() < levels; ++i) {
      result.emplace_back(prices[i], quantities[i]);
    }
    return result;
  }

  bool isEmpty() const { return prices.empty(); }
  double GetPrice() const { return prices[0]; }
};

class AskBook {
 private:
  std::vector<double> prices;
  std::vector<int> quantities;

 public:
  void addOrder(const std::shared_ptr<Order>& order) {
    const auto& it =
        std::lower_bound(prices.begin(), prices.end(), order->price);
    const size_t& index = std::distance(prices.begin(), it);
    if (it != prices.end() && *it == order->price) {
      quantities[index] += order->quantity;
    } else {
      prices.insert(it, order->price);
      quantities.insert(quantities.begin() + index, order->quantity);
    }
  }

  void removeOrder(double price, int quantity) {
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

  std::vector<std::pair<double, int>> getTopOfBook(int levels = 5) const {
    std::vector<std::pair<double, int>> result;
    for (size_t i = 0; i < prices.size() && result.size() < levels; ++i) {
      result.emplace_back(prices[i], quantities[i]);
    }
    return result;
  }

  bool isEmpty() const { return prices.empty(); }
  double GetPrice() const { return prices[0]; }
};

class OrderBook {
 private:
  BidBook bidBook;
  AskBook askBook;

 public:
  void addOrder(const std::shared_ptr<Order>& order) {
    order->side == OrderSide::BUY ? bidBook.addOrder(order)
                                  : askBook.addOrder(order);
  }

  void removeOrder(const std::shared_ptr<Order>& order) {
    order->side == OrderSide::BUY
        ? bidBook.removeOrder(order->price, order->quantity)
        : askBook.removeOrder(order->price, order->quantity);
  }
  BidBook GetBidBook() { return bidBook; }
  AskBook GetAskBook() { return askBook; }
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

  std::shared_ptr<Order> generateRandomOrder() {
    const OrderType& type =
        (rng() % 2 == 0) ? OrderType::MARKET : OrderType::LIMIT;
    const OrderSide& side = (rng() % 2 == 0) ? OrderSide::BUY : OrderSide::SELL;
    const double& price = 100.0 + (rng() % 1000) / 100.0;
    const int& quantity = 1 + (rng() % 100);

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

  void printOrderBookStatus() {
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
};

int main() {
  ExecutionEngine engine;
  const auto& start_time = std::chrono::high_resolution_clock::now();
  engine.simulateTrading(10000);
  const auto& end_time = std::chrono::high_resolution_clock::now();
  std::cout << "Simulation took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                     start_time)
                   .count()
            << " ms" << '\n';
  return 0;
}
