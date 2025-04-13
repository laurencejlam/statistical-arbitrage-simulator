#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "MarketData.h"
#include "AssetPair.h"

class Backtester {
public:
    struct Position {
        std::string symbolA;
        std::string symbolB;
        double quantityA = 0.0;
        double quantityB = 0.0;
        double entryPriceA = 0.0;
        double entryPriceB = 0.0;
        int entryDay = -1;
        int direction = 0;  // 1 for long spread, -1 for short spread
    };
    
    struct PerformanceMetrics {
        double totalReturn = 0.0;
        double annualizedReturn = 0.0;
        double sharpeRatio = 0.0;
        double maxDrawdown = 0.0;
        int winCount = 0;
        int lossCount = 0;
        double avgHoldingPeriod = 0.0;
        double avgWin = 0.0;
        double avgLoss = 0.0;
    };
    
    // Constructor with market data
    explicit Backtester(std::shared_ptr<MarketData> marketData);
    
    // Add a pair to backtest
    void addPair(const std::string& symbolA, const std::string& symbolB);
    
    // Run the backtest with specified parameters
    void runBacktest(double initialCapital = 1000000.0,
                    double entryThreshold = 1.5,
                    double exitThreshold = 0.0,
                    size_t lookbackWindow = 20,
                    bool delayedExecution = true);
    
    // Get daily portfolio values
    const std::vector<double>& getPortfolioValues() const { return portfolioValues_; }
    
    // Get trade history
    const std::vector<std::pair<int, double>>& getTradeHistory() const { return tradeHistory_; }
    
    // Get performance metrics
    PerformanceMetrics getPerformanceMetrics() const { return metrics_; }
    
    // Export results to CSV
    bool exportResults(const std::string& filename) const;

private:
    std::shared_ptr<MarketData> marketData_;
    std::vector<std::unique_ptr<AssetPair>> pairs_;
    std::vector<Position> positions_;
    std::vector<double> portfolioValues_;
    std::vector<std::pair<int, double>> tradeHistory_; // (day, pnl)
    double cash_ = 0.0;
    double initialCapital_ = 0.0;
    PerformanceMetrics metrics_;
    
    // Calculate portfolio value on a specific day
    double calculatePortfolioValue(int day);
    
    // Execute a buy/sell order
    void executeOrder(int day, const std::string& symbol, double quantity, double price);
    
    // Calculate performance metrics
    void calculateMetrics();
}; 