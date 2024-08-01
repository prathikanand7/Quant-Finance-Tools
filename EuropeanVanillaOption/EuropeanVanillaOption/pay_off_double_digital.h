#pragma once

#include <algorithm>

#include "pay_off.h"

class PayOffDoubleDigital : public PayOff {
 private:
  double U;  // Upper strike price
  double D;  // Lower strike price

 public:
  // Two strike parameters
  PayOffDoubleDigital(const double &_U, const double &_D);
  virtual ~PayOffDoubleDigital();  // Virtual destructor

  // Pay-off is 1 if S is between U and D, 0 otherwise
  double operator()(const double &S) const override;
};