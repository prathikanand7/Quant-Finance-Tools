#pragma once

#include <algorithm>

#include "pay_off.h"

class PayOffCall : public PayOff {
 private:
  double K;  // Strike price

 public:
  PayOffCall(const double& _K);     // Parametrized constructor
  virtual ~PayOffCall() = default;  // Virtual destructor

  // Virtual function is now over−ridden (not pure−virtual anymore)
  // Pay-off is max(S-K,0) for a call option

  double operator()(const double& S) const override;
};