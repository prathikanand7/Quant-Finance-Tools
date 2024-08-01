#pragma once

#include <algorithm>

class PayOff {
 public:
  PayOff() = default;           // Default constructor (no parameters)
  virtual ~PayOff() = default;  // Virtual destructor

  // Overload operator() to turn the PayOff into an abstract class

  virtual double operator()(const double& S) const = 0;  // Pure Virtual method
};