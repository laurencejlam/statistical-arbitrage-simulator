#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

class MarketData {
public:
    struct TimeSeriesData {
        std::vector<std::string> dates;
        std::unordered_map<std::string, std::vector<double>> prices;
    };

    // Constructor with filepath
    explicit MarketData(const std::string& dataFilePath);
    
    // Default constructor
    MarketData() = default;
    
    // Load market data from CSV
    bool loadFromCSV(const std::string& filePath);
    
    // Get price series for a specific asset
    std::optional<std::vector<double>> getPriceSeries(const std::string& symbol) const;
    
    // Get date series
    const std::vector<std::string>& getDateSeries() const;
    
    // Get all available symbols
    std::vector<std::string> getAvailableSymbols() const;
    
    // Get number of trading days in the dataset
    size_t getDataSize() const;

private:
    TimeSeriesData data_;
    bool isDataLoaded_ = false;
}; 