#include "pay_off_put.h"

PayOffPut::PayOffPut(const double& _K) { K = _K; }

double PayOffPut::operator()(const double& S) const {
  return std::max(K - S, 0.0);
}