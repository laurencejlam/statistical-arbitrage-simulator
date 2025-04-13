#pragma once

#include <string>
#include <vector>
#include <optional>
#include "Utilities.h"

class AssetPair {
public:
    // Constructor with asset symbols and price data
    AssetPair(const std::string& symbolA, const std::string& symbolB,
              const std::vector<double>& pricesA, const std::vector<double>& pricesB);
    
    // Get asset symbols
    const std::string& getSymbolA() const { return symbolA_; }
    const std::string& getSymbolB() const { return symbolB_; }
    
    // Get price data
    const std::vector<double>& getPricesA() const { return pricesA_; }
    const std::vector<double>& getPricesB() const { return pricesB_; }
    
    // Get the spreads between the assets (price_A - beta * price_B)
    const std::vector<double>& getSpreads() const { return spreads_; }
    
    // Get the z-scores of the spread
    std::vector<double> getZScores(size_t window) const;
    
    // Test for cointegration
    bool testCointegration(double significanceLevel = 0.05);
    
    // Get the cointegration coefficient (beta)
    double getCointegrationBeta() const { return beta_; }
    
    // Calculate signals based on z-score thresholds
    // Returns: 1 for long spread, -1 for short spread, 0 for no position
    std::vector<int> generateSignals(double entryThreshold = 1.5, 
                                    double exitThreshold = 0.0,
                                    size_t lookbackWindow = 20) const;

private:
    std::string symbolA_;
    std::string symbolB_;
    std::vector<double> pricesA_;
    std::vector<double> pricesB_;
    std::vector<double> spreads_;
    double beta_ = 1.0;  // Default to 1.0 (simple difference)
    bool isCointegrated_ = false;
    
    // Calculate the spread based on cointegration beta
    void calculateSpreads();
}; 