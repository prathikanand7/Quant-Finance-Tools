#include "order_book.h"

#include "order.h"

void OrderBook::addOrder(const std::shared_ptr<Order>& order) {
  order->side == OrderSide::BUY ? bidBook.addOrder(order)
                                : askBook.addOrder(order);
}

void OrderBook::removeOrder(const std::shared_ptr<Order>& order) {
  order->side == OrderSide::BUY
      ? bidBook.removeOrder(order->price, order->quantity)
      : askBook.removeOrder(order->price, order->quantity);
}

BidBook OrderBook::GetBidBook() { return bidBook; }

AskBook OrderBook::GetAskBook() { return askBook; }
