# Statistical Arbitrage Simulator

A C++ implementation of a statistical arbitrage backtesting framework with econometric tools for pair selection, signal generation, and performance analysis.

## Project Overview

This project implements a complete statistical arbitrage simulation engine in C++, focusing on:

1. **Market Data Handling** - Loading and processing historical price data
2. **Econometric Analysis** - Cointegration testing and spread modeling
3. **Signal Generation** - Z-score based entry/exit signals
4. **Backtesting Engine** - Order execution and portfolio tracking
5. **Performance Metrics** - PnL, Sharpe ratio, drawdown analysis

## Strategy Explanation

The statistical arbitrage strategy implemented here is based on the concept of cointegration between asset pairs. When two assets are cointegrated, their price series maintain a long-term equilibrium relationship despite short-term deviations.

Key components:
- **Pair Selection**: Using Engle-Granger methodology to identify cointegrated pairs
- **Spread Construction**: Creating a mean-reverting spread using the cointegration coefficient
- **Signal Generation**: Generating entry/exit signals based on z-score thresholds
- **Position Management**: Taking offsetting long-short positions in the asset pair

## Requirements

- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake (version 3.10+)
- Optional: Eigen library for linear algebra operations
- Python with matplotlib for visualization (optional)

## Building on Windows

1. **Install Required Tools**:
   - Install [Visual Studio](https://visualstudio.microsoft.com/downloads/) with C++ desktop development workload
   - Install [CMake](https://cmake.org/download/)
   - Install [Python](https://www.python.org/downloads/) (for visualization)

2. **Install Python Dependencies**:
   ```
   pip install -r requirements.txt
   ```

3. **Generate Project Files**:
   ```
   mkdir build
   cd build
   cmake ..
   ```

4. **Build the Project**:
   - Open the generated `.sln` file in Visual Studio
   - Build Solution (F7)
   
   Or from command line:
   ```
   cmake --build . --config Release
   ```

5. **Download Catch2 for Tests**:
   ```
   cd tests
   curl -o catch.hpp https://github.com/catchorg/Catch2/releases/download/v2.13.6/catch.hpp
   ```

## Building on Linux/Mac

1. **Install Required Tools**:
   ```
   # Ubuntu
   sudo apt-get install build-essential cmake python3 python3-pip
   
   # macOS
   brew install cmake python3
   ```

2. **Install Python Dependencies**:
   ```
   pip3 install -r requirements.txt
   ```

3. **Build the Project**:
   ```
   mkdir build && cd build
   cmake ..
   make
   ```

4. **Setup Tests**:
   ```
   cd scripts
   bash setup_tests.sh
   ```

## Running the Simulator

```
# Windows
.\build\Release\StatArbSimulator.exe data\sample_data.csv

# Linux/Mac
./build/StatArbSimulator data/sample_data.csv
```

### Command-line Options

```
Usage: StatArbSimulator <data_file> [options]
Options:
  --capital <value>        Initial capital (default: 1000000)
  --entry <value>          Entry threshold (default: 1.5)
  --exit <value>           Exit threshold (default: 0.0)
  --window <value>         Lookback window (default: 20)
  --immediate              Use immediate execution (default: T+1)
  --output <file>          Output file for results (default: results.csv)
  --help                   Show this help message
```

## Running the Demo (Windows)

1. **Generate Sample Data**:
   ```
   cd scripts
   python generate_sample_data.py --days 504 --pairs 3 --output ..\data\sample_data.csv --plot
   ```

2. **Run Backtest**:
   ```
   .\build\Release\StatArbSimulator.exe ..\data\sample_data.csv --output ..\results\results.csv
   ```

3. **Visualize Results**:
   ```
   cd scripts
   python visualize.py ..\results\results.csv --output ..\results
   ```

## Project Structure

- **include/** - Header files
- **src/** - Implementation files
- **tests/** - Unit tests
- **data/** - Sample data files
- **scripts/** - Python visualization scripts
- **build/** - Build artifacts (created during build)
- **results/** - Output results and charts (created during execution)

## Performance Summary

*Note: Performance will depend on the specific data used and parameter settings.*

Expected metrics include:
- Total return
- Sharpe ratio
- Maximum drawdown
- Win/loss ratio
- Average holding period 