#pragma once

class BlackScholesModel {
 public:
  static double CalculateCallPrice(const double& S, const double& K,
                                   const double& T, const double& r,
                                   const double& sigma);
  static double CalculatePutPrice(const double& S, const double& K,
                                  const double& T, const double& r,
                                  const double& sigma);
};