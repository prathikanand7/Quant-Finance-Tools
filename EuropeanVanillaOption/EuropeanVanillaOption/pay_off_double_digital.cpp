#include "pay_off_double_digital.h"

PayOffDoubleDigital::PayOffDoubleDigital(const double& _U, const double& _D)
    : U(_U), D(_D) {}

PayOffDoubleDigital::~PayOffDoubleDigital() = default;

double PayOffDoubleDigital::operator()(const double& S) const {
  if (S >= D && S <= U) return 1.0;

  return 0.0;
}
