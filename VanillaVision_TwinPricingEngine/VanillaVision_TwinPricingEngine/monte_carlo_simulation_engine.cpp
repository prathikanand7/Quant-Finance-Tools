
#include "monte_carlo_simulation_engine.h"

#include <chrono>
#include <future>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

#include "pay_off.h"
#include "vanilla_option.h"

// parametrized Constructor
MonteCarloSimulation::MonteCarloSimulation(const double& S, const double& K,
                                           const double& T, const double& r,
                                           const double& sigma,
                                           OptionType optionType)
    : option_(K, r, T, S, sigma), optionType_(optionType) {}

// Single-threaded Monte Carlo simulation
std::pair<double, double> MonteCarloSimulation::RunSingleThreadedSimulation(
    const int& num_scenarios, const unsigned& seed) const {
  const auto start = std::chrono::high_resolution_clock::now();
  std::mt19937 generator(seed);
  std::normal_distribution<double> nd(0.0, 1.0);
  std::vector<double> prices(num_scenarios);

  for (int i = 0; i < num_scenarios; ++i) {
    const double epsilon = nd(generator);
    const double S_T =
        option_.GetS() *
        std::exp(
            (option_.Getr() - 0.5 * option_.Getsigma() * option_.Getsigma()) *
                option_.GetT() +
            option_.Getsigma() * std::sqrt(option_.GetT()) * epsilon);

    const double payoff = optionType_ == OptionType::Call
                              ? PayOff::PayOffCall(S_T, option_.GetK())
                              : PayOff::PayOffPut(option_.GetK(), S_T);

    prices[i] = payoff;
  }
  double averagePrice =
      std::accumulate(prices.begin(), prices.end(), 0.0) / num_scenarios;
  averagePrice *= std::exp(-option_.Getr() *
                           option_.GetT());  // Discount back to present value

  const auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  return {averagePrice, elapsed.count() * 1000};
}

// Multi-threaded Monte Carlo simulation
std::pair<double, double> MonteCarloSimulation::RunMultiThreadedSimulation(
    const int& num_scenarios, const unsigned int& seed) {
  const auto start = std::chrono::high_resolution_clock::now();

  std::vector<std::future<std::pair<double, double>>> futures;
  const int num_threads = std::thread::hardware_concurrency();
  const int num_scenarios_per_thread = num_scenarios / num_threads;

  futures.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i) {
    futures.push_back(std::async(
        std::launch::async, &MonteCarloSimulation::RunSingleThreadedSimulation,
        this, num_scenarios_per_thread, seed));
  }

  std::vector<double> all_prices;
  for (auto& future : futures) {
    const auto prices = future.get();
    all_prices.emplace_back(prices.first);
  }

  const double average_discounted_price =
      std::accumulate(all_prices.begin(), all_prices.end(), 0.0) / num_threads;

  const auto end = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  return {average_discounted_price, elapsed.count() * 1000};
}