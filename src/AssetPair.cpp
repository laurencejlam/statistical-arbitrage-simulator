#include "AssetPair.h"
#include <algorithm>
#include <iostream>

AssetPair::AssetPair(const std::string& symbolA, const std::string& symbolB,
                     const std::vector<double>& pricesA, const std::vector<double>& pricesB)
    : symbolA_(symbolA), symbolB_(symbolB), pricesA_(pricesA), pricesB_(pricesB)
{
    // Ensure both price series have the same length
    if (pricesA_.size() != pricesB_.size()) {
        // Resize to the smaller of the two sizes
        size_t minSize = std::min(pricesA_.size(), pricesB_.size());
        pricesA_.resize(minSize);
        pricesB_.resize(minSize);
        
        std::cerr << "Warning: Price series for " << symbolA_ << " and " << symbolB_ 
                  << " have different lengths. Truncating to " << minSize << std::endl;
    }
    
    // Calculate default spreads (assuming beta=1)
    calculateSpreads();
}

bool AssetPair::testCointegration(double significanceLevel) {
    // Perform linear regression of priceA on priceB
    auto regression = Utilities::linearRegression(pricesB_, pricesA_);
    
    // Update beta to regression coefficient
    beta_ = regression.beta;
    
    // Recalculate spreads with new beta
    calculateSpreads();
    
    // Test for stationarity in the spread
    auto adfResult = Utilities::adfTest(spreads_, 1);
    
    isCointegrated_ = adfResult.isStationary;
    return isCointegrated_;
}

void AssetPair::calculateSpreads() {
    spreads_.resize(pricesA_.size());
    for (size_t i = 0; i < pricesA_.size(); ++i) {
        spreads_[i] = pricesA_[i] - beta_ * pricesB_[i];
    }
}

std::vector<double> AssetPair::getZScores(size_t window) const {
    if (window >= spreads_.size()) {
        window = spreads_.size() / 2;  // Default to half the series length
        if (window < 2) window = 2;    // Minimum window size
    }
    
    return Utilities::rollingZScore(spreads_, window);
}

std::vector<int> AssetPair::generateSignals(double entryThreshold, 
                                          double exitThreshold,
                                          size_t lookbackWindow) const {
    // Get z-scores
    std::vector<double> zScores = getZScores(lookbackWindow);
    
    // Generate signals
    std::vector<int> signals(zScores.size(), 0);
    int currentPosition = 0;
    
    for (size_t i = 0; i < zScores.size(); ++i) {
        if (std::isnan(zScores[i])) {
            signals[i] = 0;  // No signal for NaN z-scores
            continue;
        }
        
        if (currentPosition == 0) {
            // No position, check for entry
            if (zScores[i] > entryThreshold) {
                // Short the spread (sell A, buy B)
                signals[i] = -1;
                currentPosition = -1;
            } else if (zScores[i] < -entryThreshold) {
                // Long the spread (buy A, sell B)
                signals[i] = 1;
                currentPosition = 1;
            }
        } else if (currentPosition == 1) {
            // Long position, check for exit
            if (zScores[i] >= -exitThreshold) {
                // Exit long position
                signals[i] = 0;
                currentPosition = 0;
            } else {
                // Maintain long position
                signals[i] = 1;
            }
        } else if (currentPosition == -1) {
            // Short position, check for exit
            if (zScores[i] <= exitThreshold) {
                // Exit short position
                signals[i] = 0;
                currentPosition = 0;
            } else {
                // Maintain short position
                signals[i] = -1;
            }
        }
    }
    
    return signals;
} 