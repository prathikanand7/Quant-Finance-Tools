#pragma once

#include "Order.h"
#include "ask_book.h"
#include "bid_book.h"

class OrderBook {
 private:
  BidBook bidBook;
  AskBook askBook;

 public:
  void addOrder(const std::shared_ptr<Order>& order);
  void removeOrder(const std::shared_ptr<Order>& order);
  BidBook GetBidBook();
  AskBook GetAskBook();
};
