#include "vanilla_option.h"

#include <cmath>
#include <iostream>

VanillaOption::VanillaOption() {
  K = 100.0;  // strike price
  r = 0.05;   // 5% interest rate
  T = 1.0;    // one year until expiry
  S = 100.0;  // Option underlying asset price, Option is 'at the money' as spot
              // equals the strike.
  sigma = 0.2;  // volatility of the underlying asset is 20%
}

VanillaOption::VanillaOption(const double &_K, const double &_r,
                             const double &_T, const double &_S,
                             const double &_sigma)
    : K(_K), r(_r), T(_T), S(_S), sigma(_sigma) {}

void VanillaOption::copy(const VanillaOption &option) {
  K = option.K;
  r = option.r;
  T = option.T;
  S = option.S;
  sigma = option.sigma;
}

VanillaOption::VanillaOption(const VanillaOption &option) { copy(option); }
VanillaOption &VanillaOption::operator=(const VanillaOption &rhs) {
  if (this == &rhs) return *this;
  copy(rhs);
  return *this;
}
VanillaOption::~VanillaOption() = default;

double VanillaOption::GetK() const { return K; }
void VanillaOption::SetK(const double &_K) { K = _K; }

double VanillaOption::Getr() const { return r; }
void VanillaOption::Setr(const double &_r) { r = _r; }

double VanillaOption::GetT() const { return T; }
void VanillaOption::SetT(const double &_T) { T = _T; }

double VanillaOption::GetS() const { return S; }
void VanillaOption::SetS(const double &_S) { S = _S; }

double VanillaOption::Getsigma() const { return sigma; }
void VanillaOption::Setsigma(const double &_sigma) { sigma = _sigma; }

double VanillaOption::CalculateCallPrice() const {
  const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) /
                    (sigma * std::sqrt(T));
  const double d2 = d1 - sigma * std::sqrt(T);

  // N_d1 and N_d2 are Normal cumulative distribution function
  const double N_d1 = 0.5 * (1.0 + std::erf(d1 / std::sqrt(2.0)));
  const double N_d2 = 0.5 * (1.0 + std::erf(d2 / std::sqrt(2.0)));
  const double call_price = S * N_d1 - K * std::exp(-r * T) * N_d2;
  return call_price;
}
double VanillaOption::CalculatePutPrice() const {
  const double d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) /
                    (sigma * std::sqrt(T));
  const double d2 = d1 - sigma * std::sqrt(T);
  const double N_d1 = 0.5 * (1.0 + std::erf(d1 / std::sqrt(2.0)));
  const double N_d2 = 0.5 * (1.0 + std::erf(d2 / std::sqrt(2.0)));
  const double put_price =
      K * std::exp(-r * T) * (1.0 - N_d2) - S * (1.0 - N_d1);
  return put_price;
}
