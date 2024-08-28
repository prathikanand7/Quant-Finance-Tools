# Trading Simulation Console Application

Welcome to the **Trading Simulation Console Application**

This project simulates a trading engine that processes orders in parallel, mimicking the behavior of a financial market order book. The application utilizes multi-threading to handle large volumes of orders efficiently and supports market, limit, and stop orders.

## Features

- **Multi-threaded Order Processing:** Leverages the power of multi-threading to process orders in parallel, improving performance on multi-core systems.
- **Order Types:** Supports Market, Limit, and Stop orders.
- **Order Book Management:** Maintains separate bid and ask books for managing orders.
- **Random Order Generation:** Simulates random order generation to mimic real-time trading activity.
- **Stop Order Triggering:** Automatically triggers stop orders based on the last trade price.

## Project Structure

The project is organized into the following files:

- **Order.h / Order.cpp:** Contains the `Order` class definition and implementation, representing an order in the market.
- **BidBook.h / BidBook.cpp:** Manages the bid side of the order book.
- **AskBook.h / AskBook.cpp:** Manages the ask side of the order book.
- **OrderBook.h / OrderBook.cpp:** Combines both bid and ask books to maintain the complete order book.
- **ExecutionEngine.h / ExecutionEngine.cpp:** Implements the core trading simulation logic, including order processing, market and limit order execution, and stop order management.
- **main.cpp:** Entry point of the application that initializes the trading engine and runs the simulation.

## Getting Started

### Prerequisites

- **C++ Compiler:** Ensure you have a C++ compiler installed, such as `msvc`, `g++` or `clang`. I built this application in `msvc` using `VS 17.8.3`
- **C++14 or Higher:** This project requires C++14 or higher for threading and other modern C++ features. I built in C++20.

### Building the Application

1. Clone the repository:

   ```bash
   git clone https://github.com/prathikanand7/Quant-finance.git
   cd order-book
   ```

2. Compile the project:
   ```bash
   g++ main.cpp order.cpp bid_book.cpp ask_book.cpp order_book.cpp execution_engine.cpp -o trading_simulation -lpthread
   ```

3. Running the Application
After compiling, you can run the application:
   ```bash
   ./trading_simulation
   ```
4. Or skip all the above mentioned steps and open the `order_book.sln` in `VS 17.8.3` and above and run the application in `Release`.

The application will simulate trading with 10,000 random orders and display the status of the order book and the execution of orders in the console.

### How It Works
- **Order Generation:** Random orders are generated and processed in parallel using multiple threads. The `ExecutionEngine` class handles this processing.
- **Order Matching:** Market and limit orders are matched against the existing order book, and stop orders are triggered based on the last trade price.
- **Multi-Threading:** The `processOrdersInParallel` function divides the order processing workload across multiple threads, allowing the simulation to run efficiently on multi-core systems.

License
This project is licensed under the MIT License - see the [LICENSE](https://github.com/prathikanand7/Quant-finance/blob/main/LICENSE.txt) file for details.
