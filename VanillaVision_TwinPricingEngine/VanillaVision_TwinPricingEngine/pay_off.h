#pragma once

class PayOff {
 public:
  static double PayOffCall(const double& S, const double& K);
  static double PayOffPut(const double& K, const double& S);
};