#include "Backtester.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>

Backtester::Backtester(std::shared_ptr<MarketData> marketData)
    : marketData_(marketData)
{
}

void Backtester::addPair(const std::string& symbolA, const std::string& symbolB) {
    auto pricesA = marketData_->getPriceSeries(symbolA);
    auto pricesB = marketData_->getPriceSeries(symbolB);
    
    if (!pricesA || !pricesB) {
        std::cerr << "Error: Could not find price data for " << symbolA
                  << " or " << symbolB << std::endl;
        return;
    }
    
    auto pair = std::make_unique<AssetPair>(symbolA, symbolB, *pricesA, *pricesB);
    
    // Test for cointegration
    if (pair->testCointegration()) {
        std::cout << "Pair " << symbolA << " / " << symbolB 
                  << " is cointegrated with beta: " << pair->getCointegrationBeta() << std::endl;
        pairs_.push_back(std::move(pair));
    } else {
        std::cout << "Pair " << symbolA << " / " << symbolB 
                  << " is not cointegrated, skipping" << std::endl;
    }
}

void Backtester::runBacktest(double initialCapital,
                           double entryThreshold,
                           double exitThreshold,
                           size_t lookbackWindow,
                           bool delayedExecution) {
    // Reset state
    cash_ = initialCapital;
    initialCapital_ = initialCapital;
    positions_.clear();
    portfolioValues_.clear();
    tradeHistory_.clear();
    
    size_t numDays = marketData_->getDataSize();
    if (numDays == 0) {
        std::cerr << "Error: No market data available" << std::endl;
        return;
    }
    
    // Initialize portfolio values with initial capital
    portfolioValues_.resize(numDays, 0.0);
    
    // For each pair, generate signals and execute trades
    for (const auto& pair : pairs_) {
        // Generate trading signals
        std::vector<int> signals = pair->generateSignals(entryThreshold, exitThreshold, lookbackWindow);
        
        int currentPosition = 0;
        Position position;
        
        for (size_t day = lookbackWindow; day < numDays; ++day) {
            int signal = (day < signals.size()) ? signals[day] : 0;
            
            // Handle signal with potential delay
            int executionDay = day;
            if (delayedExecution && day + 1 < numDays) {
                executionDay = day + 1;  // T+1 execution
            }
            
            if (signal != currentPosition) {
                if (currentPosition != 0) {
                    // Close existing position
                    double priceA = pair->getPricesA()[executionDay];
                    double priceB = pair->getPricesB()[executionDay];
                    
                    // Calculate P&L
                    double entryValueA = position.quantityA * position.entryPriceA;
                    double entryValueB = position.quantityB * position.entryPriceB;
                    double exitValueA = position.quantityA * priceA;
                    double exitValueB = position.quantityB * priceB;
                    
                    double pnl = (exitValueA - entryValueA) + (exitValueB - entryValueB);
                    tradeHistory_.emplace_back(executionDay, pnl);
                    
                    // Update cash
                    cash_ += exitValueA + exitValueB;
                    
                    // Record outcome
                    if (pnl > 0) metrics_.winCount++;
                    else metrics_.lossCount++;
                    
                    // Record holding period
                    int holdingPeriod = executionDay - position.entryDay;
                    metrics_.avgHoldingPeriod += holdingPeriod;
                    
                    // Clear position
                    position = Position();
                    currentPosition = 0;
                }
                
                if (signal != 0) {
                    // Open new position
                    double priceA = pair->getPricesA()[executionDay];
                    double priceB = pair->getPricesB()[executionDay];
                    
                    // Size position based on fixed notional value
                    // Allocate 10% of portfolio to each pair
                    double positionSize = calculatePortfolioValue(executionDay-1) * 0.1;
                    
                    // Determine position direction
                    double quantityA, quantityB;
                    if (signal > 0) {
                        // Long spread: buy A, sell B
                        quantityA = positionSize / (2 * priceA);
                        quantityB = -positionSize / (2 * priceB);
                    } else {
                        // Short spread: sell A, buy B
                        quantityA = -positionSize / (2 * priceA);
                        quantityB = positionSize / (2 * priceB);
                    }
                    
                    // Update position
                    position.symbolA = pair->getSymbolA();
                    position.symbolB = pair->getSymbolB();
                    position.quantityA = quantityA;
                    position.quantityB = quantityB;
                    position.entryPriceA = priceA;
                    position.entryPriceB = priceB;
                    position.entryDay = executionDay;
                    position.direction = signal;
                    
                    // Update cash
                    cash_ -= (quantityA * priceA + quantityB * priceB);
                    
                    // Add to active positions
                    positions_.push_back(position);
                    currentPosition = signal;
                }
            }
            
            // Update portfolio value for the day
            portfolioValues_[executionDay] = calculatePortfolioValue(executionDay);
        }
    }
    
    // Calculate performance metrics
    calculateMetrics();
}

double Backtester::calculatePortfolioValue(int day) {
    if (day < 0 || day >= static_cast<int>(marketData_->getDataSize())) {
        return initialCapital_;
    }
    
    double totalValue = cash_;
    
    // Add value of all positions
    for (const auto& position : positions_) {
        auto pricesA = marketData_->getPriceSeries(position.symbolA);
        auto pricesB = marketData_->getPriceSeries(position.symbolB);
        
        if (pricesA && pricesB && day < static_cast<int>(pricesA->size()) && day < static_cast<int>(pricesB->size())) {
            double currentPriceA = (*pricesA)[day];
            double currentPriceB = (*pricesB)[day];
            
            totalValue += position.quantityA * currentPriceA + position.quantityB * currentPriceB;
        }
    }
    
    return totalValue;
}

void Backtester::executeOrder(int day, const std::string& symbol, double quantity, double price) {
    // Update cash
    cash_ -= quantity * price;
}

void Backtester::calculateMetrics() {
    if (portfolioValues_.empty()) {
        return;
    }
    
    // Calculate total return
    double finalValue = portfolioValues_.back();
    metrics_.totalReturn = (finalValue / initialCapital_) - 1.0;
    
    // Calculate annualized return (assuming 252 trading days per year)
    double years = static_cast<double>(portfolioValues_.size()) / 252.0;
    metrics_.annualizedReturn = std::pow(1.0 + metrics_.totalReturn, 1.0 / years) - 1.0;
    
    // Calculate daily returns
    std::vector<double> dailyReturns(portfolioValues_.size() - 1);
    for (size_t i = 1; i < portfolioValues_.size(); ++i) {
        dailyReturns[i-1] = (portfolioValues_[i] / portfolioValues_[i-1]) - 1.0;
    }
    
    // Calculate Sharpe ratio (assuming 0% risk-free rate)
    double meanDailyReturn = Utilities::mean(dailyReturns);
    double stdDailyReturn = Utilities::standardDeviation(dailyReturns);
    
    if (stdDailyReturn > 0) {
        metrics_.sharpeRatio = (meanDailyReturn / stdDailyReturn) * std::sqrt(252.0);
    }
    
    // Calculate max drawdown
    double maxValue = portfolioValues_[0];
    double maxDrawdown = 0.0;
    
    for (const auto& value : portfolioValues_) {
        if (value > maxValue) {
            maxValue = value;
        } else {
            double drawdown = (maxValue - value) / maxValue;
            if (drawdown > maxDrawdown) {
                maxDrawdown = drawdown;
            }
        }
    }
    
    metrics_.maxDrawdown = maxDrawdown;
    
    // Calculate average metrics
    int totalTrades = metrics_.winCount + metrics_.lossCount;
    if (totalTrades > 0) {
        metrics_.avgHoldingPeriod /= totalTrades;
    }
    
    // Calculate average win/loss
    double totalWins = 0.0;
    double totalLosses = 0.0;
    
    for (const auto& trade : tradeHistory_) {
        double pnl = trade.second;
        if (pnl > 0) {
            totalWins += pnl;
        } else {
            totalLosses += pnl;
        }
    }
    
    if (metrics_.winCount > 0) {
        metrics_.avgWin = totalWins / metrics_.winCount;
    }
    
    if (metrics_.lossCount > 0) {
        metrics_.avgLoss = totalLosses / metrics_.lossCount;
    }
    
    // Print metrics
    std::cout << "------- Performance Metrics -------" << std::endl;
    std::cout << "Total Return: " << (metrics_.totalReturn * 100.0) << "%" << std::endl;
    std::cout << "Annualized Return: " << (metrics_.annualizedReturn * 100.0) << "%" << std::endl;
    std::cout << "Sharpe Ratio: " << metrics_.sharpeRatio << std::endl;
    std::cout << "Max Drawdown: " << (metrics_.maxDrawdown * 100.0) << "%" << std::endl;
    std::cout << "Win Rate: " << (static_cast<double>(metrics_.winCount) / totalTrades * 100.0) << "%" << std::endl;
    std::cout << "Average Holding Period: " << metrics_.avgHoldingPeriod << " days" << std::endl;
    std::cout << "Average Win: " << metrics_.avgWin << std::endl;
    std::cout << "Average Loss: " << metrics_.avgLoss << std::endl;
}

bool Backtester::exportResults(const std::string& filename) const {
    std::vector<std::string> headers = {"Day", "PortfolioValue"};
    std::vector<std::vector<double>> data(portfolioValues_.size(), std::vector<double>(2));
    
    for (size_t i = 0; i < portfolioValues_.size(); ++i) {
        data[i][0] = static_cast<double>(i);
        data[i][1] = portfolioValues_[i];
    }
    
    return Utilities::writeCSV(filename, headers, data);
} 