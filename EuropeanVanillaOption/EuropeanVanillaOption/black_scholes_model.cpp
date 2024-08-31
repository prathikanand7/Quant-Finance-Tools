#include "black_scholes_model.h"

#include <cmath>
#include <iostream>

// Calculate Call price using Black-Scholes formula.
double BlackScholesModel::CalculateCallPrice(const double& S, const double& K,
                                             const double& T, const double& r,
                                             const double& sigma) {
  const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) /
                    (sigma * std::sqrt(T));
  const double d2 = d1 - sigma * std::sqrt(T);

  // N_d1 and N_d2 are Normal cumulative distribution function
  const double N_d1 = 0.5 * (1.0 + std::erf(d1 / std::sqrt(2.0)));
  const double N_d2 = 0.5 * (1.0 + std::erf(d2 / std::sqrt(2.0)));
  const double call_price = S * N_d1 - K * std::exp(-r * T) * N_d2;
  return call_price;
}

// Calculate Put price using Black-Scholes formula.
double BlackScholesModel::CalculatePutPrice(const double& S, const double& K,
                                            const double& T, const double& r,
                                            const double& sigma) {
  const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) /
                    (sigma * std::sqrt(T));
  const double d2 = d1 - sigma * std::sqrt(T);
  const double N_d1 = 0.5 * (1.0 + std::erf(d1 / std::sqrt(2.0)));
  const double N_d2 = 0.5 * (1.0 + std::erf(d2 / std::sqrt(2.0)));
  const double put_price =
      K * std::exp(-r * T) * (1.0 - N_d2) - S * (1.0 - N_d1);
  return put_price;
}