// This file contains the 'main' function.
// Program execution begins and ends here.

#include <iostream>

#include "black_scholes_model.h"
#include "monte_carlo_simulation_engine.h"

int main() {
  std::cout << "Hello World!\n";

  constexpr double S = 100.0;    // Spot price
  constexpr double K = 100.0;    // Strike price
  constexpr double T = 1.0;      // Time to maturity
  constexpr double r = 0.05;     // Risk-free rate
  constexpr double sigma = 0.2;  // Volatility

  // 1 million scenarios
  constexpr int num_scenarios = 1000000;
  constexpr unsigned int seed = 42;

  // Output the simulation parameters
  std::cout << "Number of scenarios = " << num_scenarios << '\n';

  // Create MonteCarloSimulation for Call option
  MonteCarloSimulation call_simulation(S, K, T, r, sigma, OptionType::Call);

  // Single-threaded simulation for Call
  auto [callPriceSingleThreaded, callTimeSingleThreaded] =
      call_simulation.RunSingleThreadedSimulation(num_scenarios, seed);

  std::cout << "Runtime (Scenarios DID NOT run in parallel) = "
            << callTimeSingleThreaded
            << "ms; price = " << callPriceSingleThreaded << '\n'
            << '\n';

  // Multi-threaded simulation for Call
  auto [callPriceMultiThreaded, callTimeMultiThreaded] =
      call_simulation.RunMultiThreadedSimulation(num_scenarios, seed);

  std::cout << "Runtime (Scenarios RUN in parallel) = " << callTimeMultiThreaded
            << "ms; price = " << callPriceMultiThreaded << '\n'
            << '\n';

  const auto bs_call_price =
      BlackScholesModel::CalculateCallPrice(S, K, T, r, sigma);
  std::cout << "Black-Scholes Call Price: " << bs_call_price << '\n' << '\n';

  // Compare the two
  const double difference = std::abs(bs_call_price - callPriceMultiThreaded);
  std::cout << "Difference between Black-Scholes and Monte Carlo "
               "multi-threaded simulation: "
            << difference << '\n';

  // Compare the two
  const double difference_single =
      std::abs(bs_call_price - callPriceSingleThreaded);
  std::cout << "Difference between Black-Scholes and Monte Carlo "
               "single-threaded simulation: "
            << difference_single << '\n';

  //// Create MonteCarloSimulation for Put option
  // MonteCarloSimulation putSimulation(S, K, T, r, sigma, OptionType::Put);

  //// Single-threaded simulation for Put
  // auto [putPriceSingleThreaded, putTimeSingleThreaded] =
  //     putSimulation.RunSingleThreadedSimulation(num_scenarios, seed);
  // std::cout << "Runtime (Scenarios DID NOT run in parallel) = "
  //           << putTimeSingleThreaded << "ms; price = " <<
  //           putPriceSingleThreaded
  //           << '\n'
  //           << '\n';

  //// Multi-threaded simulation for Put
  // auto [putPriceMultiThreaded, putTimeMultiThreaded] =
  //     putSimulation.RunMultiThreadedSimulation(num_scenarios, seed);
  // std::cout << "Runtime (Scenarios RAN in parallel) = " <<
  // putTimeMultiThreaded
  //           << "ms; price = " << putPriceMultiThreaded << '\n'
  //           << '\n';

  return 0;
}
