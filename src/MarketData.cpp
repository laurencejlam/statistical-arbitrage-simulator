#include "MarketData.h"
#include <fstream>
#include <sstream>
#include <iostream>

MarketData::MarketData(const std::string& dataFilePath) {
    loadFromCSV(dataFilePath);
}

bool MarketData::loadFromCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return false;
    }
    
    // Read header line to get symbols
    std::string headerLine;
    if (!std::getline(file, headerLine)) {
        std::cerr << "Error: Empty file or could not read header" << std::endl;
        return false;
    }
    
    std::stringstream headerStream(headerLine);
    std::string cell;
    std::vector<std::string> headers;
    
    // Parse header row
    while (std::getline(headerStream, cell, ',')) {
        headers.push_back(cell);
    }
    
    if (headers.size() < 2) {
        std::cerr << "Error: Invalid header format, expected at least date and one symbol" << std::endl;
        return false;
    }
    
    // Initialize price vectors for each symbol
    for (size_t i = 1; i < headers.size(); ++i) {
        data_.prices[headers[i]] = std::vector<double>();
    }
    
    // Read data rows
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream lineStream(line);
        std::string dateStr;
        
        // First column is the date
        if (!std::getline(lineStream, dateStr, ',')) {
            continue;
        }
        
        data_.dates.push_back(dateStr);
        
        // Read prices for each symbol
        for (size_t i = 1; i < headers.size(); ++i) {
            std::string priceStr;
            if (!std::getline(lineStream, priceStr, ',')) {
                // Missing data, use NaN or previous value
                data_.prices[headers[i]].push_back(std::numeric_limits<double>::quiet_NaN());
            } else {
                try {
                    double price = std::stod(priceStr);
                    data_.prices[headers[i]].push_back(price);
                } catch (const std::exception& e) {
                    data_.prices[headers[i]].push_back(std::numeric_limits<double>::quiet_NaN());
                }
            }
        }
    }
    
    isDataLoaded_ = true;
    return true;
}

std::optional<std::vector<double>> MarketData::getPriceSeries(const std::string& symbol) const {
    if (!isDataLoaded_) {
        return std::nullopt;
    }
    
    auto it = data_.prices.find(symbol);
    if (it == data_.prices.end()) {
        return std::nullopt;
    }
    
    return it->second;
}

const std::vector<std::string>& MarketData::getDateSeries() const {
    return data_.dates;
}

std::vector<std::string> MarketData::getAvailableSymbols() const {
    std::vector<std::string> symbols;
    for (const auto& pair : data_.prices) {
        symbols.push_back(pair.first);
    }
    return symbols;
}

size_t MarketData::getDataSize() const {
    return data_.dates.size();
} 