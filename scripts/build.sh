#!/bin/bash
# Build script for the Statistical Arbitrage Simulator

# Create build directory if it doesn't exist
mkdir -p ../build
cd ../build

# Run CMake
echo "Running CMake..."
cmake ..

# Build the project
echo "Building project..."
make -j4

if [ $? -eq 0 ]; then
    echo "Build successful."
    echo "You can now run the simulator with: ./StatArbSimulator /path/to/data.csv"
else
    echo "Build failed. Please check for errors and try again."
    exit 1
fi 