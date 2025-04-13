#pragma once

#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>

namespace Utilities {
    // Basic statistical functions
    double mean(const std::vector<double>& data);
    double standardDeviation(const std::vector<double>& data);
    std::vector<double> calculateReturns(const std::vector<double>& prices);
    
    // Rolling window calculations
    std::vector<double> rollingMean(const std::vector<double>& data, size_t window);
    std::vector<double> rollingStdDev(const std::vector<double>& data, size_t window);
    std::vector<double> rollingZScore(const std::vector<double>& data, size_t window);
    
    // Linear regression
    struct RegressionResult {
        double alpha;  // intercept
        double beta;   // slope
        double rsquared;
        std::vector<double> residuals;
    };
    
    RegressionResult linearRegression(const std::vector<double>& x, const std::vector<double>& y);
    
    // Augmented Dickey-Fuller test for stationarity
    struct AdfResult {
        double testStatistic;
        double pValue;
        bool isStationary;
    };
    
    AdfResult adfTest(const std::vector<double>& timeSeries, int maxLags = 1);
    
    // CSV utilities
    bool writeCSV(const std::string& filename, 
                  const std::vector<std::string>& headers, 
                  const std::vector<std::vector<double>>& data);
    
    // Print utilities for debugging
    void printVector(const std::vector<double>& vec, const std::string& label = "");
} 