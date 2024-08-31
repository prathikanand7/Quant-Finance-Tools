#pragma once

#include <utility>

#include "vanilla_option.h"

enum class OptionType { Call, Put };

class MonteCarloSimulation {
 private:
  VanillaOption option_;
  OptionType optionType_;

 public:
  MonteCarloSimulation(const double& S, const double& K, const double& T,
                       const double& r, const double& sigma,
                       OptionType optionType);
  std::pair<double, double> RunSingleThreadedSimulation(
      const int& num_scenarios, const unsigned int& seed) const;
  std::pair<double, double> RunMultiThreadedSimulation(
      const int& num_scenarios, const unsigned int& seed);
};
