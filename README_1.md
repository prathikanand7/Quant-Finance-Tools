# VanillaVision: Twin Pricing Engine

## **Overview**

VanillaVision is a high-performance C++ application designed to price European vanilla options using two distinct methods: the Black-Scholes analytical solution and Monte Carlo simulation.
This twin-engine approach allows for precise pricing and method comparison, making it an invaluable tool for quantitative analysts, financial engineers, and risk managers.

The application leverages the power of parallel computation by comparing single-threaded and multi-threaded Monte Carlo simulations, highlighting the difference in performance and accuracy.

## **Key Features**

- **Dual Pricing Methods**:
  - Black-Scholes analytical solution
  - Monte Carlo simulation

- **Option Types**:
  - European Call Options
  - European Put Options

- **Flexible Parameters**: Customizable inputs for spot price, strike price, time to maturity, risk-free rate, and volatility
- **Precision Control**: Adjustable number of scenarios for Monte Carlo simulation

- **Black-Scholes Pricing Model**: Implements the `Analytical solution` for pricing European call and put options.
- **Monte Carlo Simulation**: A stochastic method that uses the `Mersenne Twister algorithm` for sampling to estimate the price of options.
- **Parallel Computation**: Leverages `Multi-threading` to speed up the Monte Carlo simulation.
- **Performance Metrics**: Compares the runtime and accuracy of different simulation methods.
- **Comparative Analysis**: Direct comparison between analytical and simulated results

## **Code Structure**

- **main.cpp**: The main entry point of the application. It sets up the simulation parameters, runs the simulations, and compares their results.
- **monte_carlo_simulation_engine.h/cpp**: Implements the Monte Carlo simulation engine, providing both single-threaded and multi-threaded execution.
- **black_scholes_model.h/cpp**: Implements the Black-Scholes analytical model for pricing European call and put options.
- **pay_off.h/cpp**: Contains classes for calculating the payoff of options (e.g., call, put).
- **vanilla_option.h/cpp**: Defines the `VanillaOption` class, which stores the parameters of the option (e.g., strike price, volatility).

## **Compilation and Execution**

To compile and run this application using `Microsoft Visual Studio 17.8.3`:

1. **Open the Solution File**:
   - Open the `EuropeanVanillaOption.sln` file in Microsoft Visual Studio.

2. **Build the Solution**:
   - Go to `Build > Build Solution` or press `Ctrl + Shift + B`. Ensure that you are building for the correct platform (`x64` or `x86`).

3. **Run the Application**:
   - After building, run the application by selecting `Release` or pressing `ctrl + F5`.

## **Usage**

To modify the pricing parameters, edit the following mentioned constants in `main.cpp`

The application will simulate the pricing of a European vanilla call option under the following conditions:

- **Strike Price (K)**: 100
- **Risk-Free Rate (r)**: 5%
- **Time to Maturity (T)**: 1 year
- **Spot Price (S)**: 100
- **Volatility (σ)**: 20%
- **Number of Scenarios**: 1,000,000

The application will output:

- The runtime and price calculated using the Black-Scholes model.
- The runtime and price calculated using a single-threaded Monte Carlo simulation.
- The runtime and price calculated using a multi-threaded Monte Carlo simulation.
- The difference between the prices calculated by the Black-Scholes model and the Monte Carlo simulations.

### **Example Output**

```plaintext
Number of scenarios = 1000000

Runtime (Scenarios DID NOT run in parallel) = 34.5946ms; price = 10.4682

Runtime (Scenarios RUN in parallel) = 5.374ms; price = 10.4604

Black-Scholes Call Price: 10.4506

Difference between Black-Scholes and Monte Carlo multi-threaded simulation: 0.00979348
Difference between Black-Scholes and Monte Carlo single-threaded simulation: 0.017616
```

### **Acknowledgments**

- Michael Halls-Moore (2010). "C++ For Quantitative Finance". Book on Quant Finance. Chapter-3,4 and 10.
- Black, Fischer; Scholes, Myron (1973). "The Pricing of Options and Corporate Liabilities". Journal of Political Economy. 81 (3): 637–654.
- Boyle, Phelim P. (1977). "Options: A Monte Carlo Approach". Journal of Financial Economics. 4 (3): 323–338.

### **Disclaimer**
VanillaVision is developed for educational and informational purposes only. It should not be used for actual trading or financial decision-making without proper understanding and professional advice.