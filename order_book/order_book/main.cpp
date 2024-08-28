#include <chrono>
#include <iostream>

#include "execution_engine.h"
int main() {
  ExecutionEngine engine;
  const auto& start_time = std::chrono::high_resolution_clock::now();
  engine.simulateTrading(10000);
  const auto& end_time = std::chrono::high_resolution_clock::now();
  std::cout << "Simulation took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                     start_time)
                   .count()
            << " ms" << '\n';
  return 0;
}