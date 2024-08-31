#include "vanilla_option.h"

#include "black_scholes_model.h"

// Default constructor
VanillaOption::VanillaOption() {
  K = 100.0;  // strike price
  r = 0.05;   // 5% interest rate
  T = 1.0;    // one year until expiry
  S = 100.0;  // Option underlying asset price, Option is 'at the money' as spot
              // equals the strike.
  sigma = 0.2;  // volatility of the underlying asset is 20%
}

// parametrized Constructor
VanillaOption::VanillaOption(const double& _K, const double& _r,
                             const double& _T, const double& _S,
                             const double& _sigma)
    : K(_K), r(_r), T(_T), S(_S), sigma(_sigma) {}

// Copy constructor
void VanillaOption::copy(const VanillaOption& option) {
  K = option.K;
  r = option.r;
  T = option.T;
  S = option.S;
  sigma = option.sigma;
}

// Copy method
VanillaOption::VanillaOption(const VanillaOption& option) { copy(option); }

// Assignment operator
VanillaOption& VanillaOption::operator=(const VanillaOption& rhs) {
  if (this == &rhs) return *this;
  copy(rhs);
  return *this;
}
VanillaOption::~VanillaOption() = default;

double VanillaOption::GetK() const { return K; }
void VanillaOption::SetK(const double& _K) { K = _K; }

double VanillaOption::Getr() const { return r; }
void VanillaOption::Setr(const double& _r) { r = _r; }

double VanillaOption::GetT() const { return T; }
void VanillaOption::SetT(const double& _T) { T = _T; }

double VanillaOption::GetS() const { return S; }
void VanillaOption::SetS(const double& _S) { S = _S; }

double VanillaOption::Getsigma() const { return sigma; }
void VanillaOption::Setsigma(const double& _sigma) { sigma = _sigma; }

// Calculate Call prices using Black-Scholes formula.
double VanillaOption::CalculateCallPrice() const {
  return BlackScholesModel::CalculateCallPrice(S, K, T, r, sigma);
}

// Calculate Put prices using Black-Scholes formula.
double VanillaOption::CalculatePutPrice() const {
  return BlackScholesModel::CalculatePutPrice(S, K, T, r, sigma);
}