#include "Order.h"

Order::Order(int id, OrderType type, OrderSide side, double price, int quantity)
    : id(id), type(type), side(side), price(price), quantity(quantity) {}
