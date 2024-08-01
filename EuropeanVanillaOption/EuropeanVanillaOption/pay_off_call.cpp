#include "pay_off_call.h"

PayOffCall::PayOffCall(const double& _K) { K = _K; }

// Overridden operator() method, which turns PayOffCall into a function object
double PayOffCall::operator()(const double& S) const {
  return std::max(S - K, 0.0);
}
