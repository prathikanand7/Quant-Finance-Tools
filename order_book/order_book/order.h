#pragma once
enum class OrderType { MARKET, LIMIT, STOP };
enum class OrderSide { BUY, SELL };

class Order {
 public:
  Order(int id, OrderType type, OrderSide side, double price, int quantity);

  int id;
  OrderType type;
  OrderSide side;
  double price;
  int quantity;
};
