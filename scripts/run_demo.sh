#!/bin/bash
# Complete demonstration script for the Statistical Arbitrage Simulator

echo "Statistical Arbitrage Simulator Demo"
echo "==================================="
echo

# Directory setup
ROOT_DIR=$(pwd)/..
BUILD_DIR=$ROOT_DIR/build
DATA_DIR=$ROOT_DIR/data
RESULTS_DIR=$ROOT_DIR/results

# Create directories if they don't exist
mkdir -p $BUILD_DIR
mkdir -p $DATA_DIR
mkdir -p $RESULTS_DIR

# Step 1: Generate sample data
echo "Step 1: Generating sample price data..."
python3 generate_sample_data.py --days 504 --pairs 3 --output $DATA_DIR/sample_data.csv --plot

if [ $? -ne 0 ]; then
    echo "Error generating sample data. Exiting."
    exit 1
fi

echo "Sample data generated successfully in $DATA_DIR/sample_data.csv"
echo

# Step 2: Build the simulator
echo "Step 2: Building the simulator..."
cd $ROOT_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake ..
make -j4

if [ $? -ne 0 ]; then
    echo "Error building simulator. Exiting."
    exit 1
fi

echo "Simulator built successfully."
echo

# Step 3: Run the backtest
echo "Step 3: Running statistical arbitrage backtest..."
cd $BUILD_DIR
./StatArbSimulator $DATA_DIR/sample_data.csv --output $RESULTS_DIR/results.csv

if [ $? -ne 0 ]; then
    echo "Error running backtest. Exiting."
    exit 1
fi

echo "Backtest completed successfully."
echo

# Step 4: Visualize results
echo "Step 4: Visualizing backtest results..."
cd $ROOT_DIR/scripts
python3 visualize.py $RESULTS_DIR/results.csv --output $RESULTS_DIR

if [ $? -ne 0 ]; then
    echo "Error visualizing results. Exiting."
    exit 1
fi

echo "Results visualized successfully."
echo

echo "Demo completed successfully!"
echo "Results are available in $RESULTS_DIR"
echo "You can view the charts in $RESULTS_DIR" 