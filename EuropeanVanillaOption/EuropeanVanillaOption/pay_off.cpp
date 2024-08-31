#include "pay_off.h"

#include <algorithm>

// Pay-off is max(S-K,0) for a call option
double PayOff::PayOffCall(const double& S, const double& K) {
  return std::max(S - K, 0.0);
}
// Pay-off is max(K-S,0) for a put option
double PayOff::PayOffPut(const double& K, const double& S) {
  return std::max(K - S, 0.0);
}