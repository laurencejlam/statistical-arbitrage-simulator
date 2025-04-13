#include "Utilities.h"
#include <cmath>
#include <limits>
#include <random>

namespace Utilities {

double mean(const std::vector<double>& data) {
    if (data.empty()) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double standardDeviation(const std::vector<double>& data) {
    if (data.size() < 2) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    
    double avg = mean(data);
    double sum_squared_diff = 0.0;
    
    for (const auto& value : data) {
        double diff = value - avg;
        sum_squared_diff += diff * diff;
    }
    
    return std::sqrt(sum_squared_diff / (data.size() - 1));
}

std::vector<double> calculateReturns(const std::vector<double>& prices) {
    if (prices.size() < 2) {
        return {};
    }
    
    std::vector<double> returns(prices.size() - 1);
    for (size_t i = 1; i < prices.size(); ++i) {
        returns[i-1] = (prices[i] / prices[i-1]) - 1.0;
    }
    
    return returns;
}

std::vector<double> rollingMean(const std::vector<double>& data, size_t window) {
    if (data.size() < window) {
        return std::vector<double>(data.size(), std::numeric_limits<double>::quiet_NaN());
    }
    
    std::vector<double> result(data.size(), std::numeric_limits<double>::quiet_NaN());
    
    for (size_t i = window - 1; i < data.size(); ++i) {
        double sum = 0.0;
        for (size_t j = i - window + 1; j <= i; ++j) {
            sum += data[j];
        }
        result[i] = sum / window;
    }
    
    return result;
}

std::vector<double> rollingStdDev(const std::vector<double>& data, size_t window) {
    if (data.size() < window) {
        return std::vector<double>(data.size(), std::numeric_limits<double>::quiet_NaN());
    }
    
    std::vector<double> result(data.size(), std::numeric_limits<double>::quiet_NaN());
    
    for (size_t i = window - 1; i < data.size(); ++i) {
        std::vector<double> window_data(data.begin() + (i - window + 1), data.begin() + i + 1);
        result[i] = standardDeviation(window_data);
    }
    
    return result;
}

std::vector<double> rollingZScore(const std::vector<double>& data, size_t window) {
    if (data.size() < window) {
        return std::vector<double>(data.size(), std::numeric_limits<double>::quiet_NaN());
    }
    
    std::vector<double> means = rollingMean(data, window);
    std::vector<double> stdDevs = rollingStdDev(data, window);
    
    std::vector<double> zScores(data.size(), std::numeric_limits<double>::quiet_NaN());
    
    for (size_t i = window - 1; i < data.size(); ++i) {
        if (stdDevs[i] > 0) {
            zScores[i] = (data[i] - means[i]) / stdDevs[i];
        }
    }
    
    return zScores;
}

RegressionResult linearRegression(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.empty()) {
        return {0, 0, 0, {}};
    }
    
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_xx = 0.0, sum_yy = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_xx += x[i] * x[i];
        sum_yy += y[i] * y[i];
    }
    
    double n = static_cast<double>(x.size());
    double denominator = n * sum_xx - sum_x * sum_x;
    
    if (denominator == 0) {
        return {0, 0, 0, {}};
    }
    
    double beta = (n * sum_xy - sum_x * sum_y) / denominator;
    double alpha = (sum_y - beta * sum_x) / n;
    
    // Calculate residuals and R-squared
    std::vector<double> residuals(x.size());
    double ss_total = 0.0;
    double ss_residual = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        double predicted = alpha + beta * x[i];
        residuals[i] = y[i] - predicted;
        ss_total += (y[i] - sum_y / n) * (y[i] - sum_y / n);
        ss_residual += residuals[i] * residuals[i];
    }
    
    double rsquared = 0.0;
    if (ss_total > 0) {
        rsquared = 1.0 - (ss_residual / ss_total);
    }
    
    return {alpha, beta, rsquared, residuals};
}

AdfResult adfTest(const std::vector<double>& timeSeries, int maxLags) {
    // This is a simplified ADF test implementation
    // In a real-world scenario, consider using an established econometrics library
    
    // Ensure we have enough data
    if (timeSeries.size() < 20) {
        return {0.0, 1.0, false};
    }
    
    // Create differenced series (y_t - y_{t-1})
    std::vector<double> diff(timeSeries.size() - 1);
    for (size_t i = 1; i < timeSeries.size(); ++i) {
        diff[i-1] = timeSeries[i] - timeSeries[i-1];
    }
    
    // Create lagged series (y_{t-1})
    std::vector<double> lagged(timeSeries.size() - 1);
    for (size_t i = 0; i < timeSeries.size() - 1; ++i) {
        lagged[i] = timeSeries[i];
    }
    
    // Run regression: diff ~ alpha + beta*lagged
    auto regression = linearRegression(lagged, diff);
    
    // Calculate t-statistic
    double beta = regression.beta;
    double se = 0.0;
    
    for (const auto& residual : regression.residuals) {
        se += residual * residual;
    }
    
    se = std::sqrt(se / (regression.residuals.size() - 2));
    se = se / std::sqrt(std::accumulate(lagged.begin(), lagged.end(), 0.0, 
                                      [](double sum, double val) { return sum + val * val; }));
    
    double tStat = beta / se;
    
    // Critical values (approximate) for 5% significance
    const double criticalValue = -2.86;
    
    // Create result
    AdfResult result;
    result.testStatistic = tStat;
    result.pValue = 0.05;  // Simplified; real implementation would compute actual p-value
    result.isStationary = (tStat < criticalValue);
    
    return result;
}

bool writeCSV(const std::string& filename, 
              const std::vector<std::string>& headers, 
              const std::vector<std::vector<double>>& data) {
    if (headers.empty() || data.empty() || data[0].size() != headers.size()) {
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    for (size_t i = 0; i < headers.size(); ++i) {
        file << headers[i];
        if (i < headers.size() - 1) {
            file << ",";
        }
    }
    file << "\n";
    
    // Write data
    for (size_t row = 0; row < data.size(); ++row) {
        for (size_t col = 0; col < data[row].size(); ++col) {
            file << data[row][col];
            if (col < data[row].size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }
    
    return true;
}

void printVector(const std::vector<double>& vec, const std::string& label) {
    if (!label.empty()) {
        std::cout << label << ": ";
    }
    
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

} 