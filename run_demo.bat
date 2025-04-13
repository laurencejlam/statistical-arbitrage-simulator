@echo off
rem Demo script for Windows users

echo Statistical Arbitrage Simulator Demo
echo ===================================
echo.

rem Create required directories
mkdir data 2>nul
mkdir results 2>nul
mkdir build 2>nul

rem Step 1: Generate sample data
echo Step 1: Generating sample price data...
cd scripts
python generate_sample_data.py --days 504 --pairs 3 --output ..\data\sample_data.csv --plot
if %ERRORLEVEL% neq 0 (
    echo Error generating sample data. Exiting.
    exit /b 1
)
cd ..

echo Sample data generated successfully in data\sample_data.csv
echo.

rem Step 2: Build the simulator
echo Step 2: Building the simulator...
cd build
cmake ..
if %ERRORLEVEL% neq 0 (
    echo Error running CMake. Exiting.
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Error building simulator. Exiting.
    exit /b 1
)
cd ..

echo Simulator built successfully.
echo.

rem Step 3: Run the backtest
echo Step 3: Running statistical arbitrage backtest...
.\build\Release\StatArbSimulator.exe data\sample_data.csv --output results\results.csv
if %ERRORLEVEL% neq 0 (
    echo Error running backtest. Exiting.
    exit /b 1
)

echo Backtest completed successfully.
echo.

rem Step 4: Visualize results
echo Step 4: Visualizing backtest results...
cd scripts
python visualize.py ..\results\results.csv --output ..\results
if %ERRORLEVEL% neq 0 (
    echo Error visualizing results. Exiting.
    exit /b 1
)
cd ..

echo Results visualized successfully.
echo.

echo Demo completed successfully!
echo Results are available in the results directory
echo.

pause 