#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "MarketData.h"
#include "AssetPair.h"
#include "Backtester.h"

void printUsage() {
    std::cout << "Usage: StatArbSimulator <data_file> [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --capital <value>        Initial capital (default: 1000000)" << std::endl;
    std::cout << "  --entry <value>          Entry threshold (default: 1.5)" << std::endl;
    std::cout << "  --exit <value>           Exit threshold (default: 0.0)" << std::endl;
    std::cout << "  --window <value>         Lookback window (default: 20)" << std::endl;
    std::cout << "  --immediate              Use immediate execution (default: T+1)" << std::endl;
    std::cout << "  --output <file>          Output file for results (default: results.csv)" << std::endl;
    std::cout << "  --help                   Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Missing data file path" << std::endl;
        printUsage();
        return 1;
    }
    
    // Parse command line arguments
    std::string dataFilePath = argv[1];
    double initialCapital = 1000000.0;
    double entryThreshold = 1.5;
    double exitThreshold = 0.0;
    size_t lookbackWindow = 20;
    bool delayedExecution = true;
    std::string outputFile = "results.csv";
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--capital" && i + 1 < argc) {
            initialCapital = std::stod(argv[++i]);
        } else if (arg == "--entry" && i + 1 < argc) {
            entryThreshold = std::stod(argv[++i]);
        } else if (arg == "--exit" && i + 1 < argc) {
            exitThreshold = std::stod(argv[++i]);
        } else if (arg == "--window" && i + 1 < argc) {
            lookbackWindow = std::stoul(argv[++i]);
        } else if (arg == "--immediate") {
            delayedExecution = false;
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }
    
    // Load market data
    std::cout << "Loading market data from " << dataFilePath << std::endl;
    auto marketData = std::make_shared<MarketData>(dataFilePath);
    
    if (marketData->getDataSize() == 0) {
        std::cerr << "Error: Failed to load market data" << std::endl;
        return 1;
    }
    
    std::cout << "Loaded " << marketData->getDataSize() << " days of data for "
              << marketData->getAvailableSymbols().size() << " symbols" << std::endl;
    
    // Create backtester
    Backtester backtester(marketData);
    
    // If we have symbols, auto-add all valid pairs
    auto symbols = marketData->getAvailableSymbols();
    if (symbols.size() >= 2) {
        std::cout << "Analyzing all possible pairs for cointegration..." << std::endl;
        
        // Test all symbol pairs for cointegration
        for (size_t i = 0; i < symbols.size(); ++i) {
            for (size_t j = i + 1; j < symbols.size(); ++j) {
                backtester.addPair(symbols[i], symbols[j]);
            }
        }
    } else {
        std::cerr << "Error: Need at least 2 symbols for pairs trading" << std::endl;
        return 1;
    }
    
    // Run backtest
    std::cout << "\nRunning backtest with parameters:" << std::endl;
    std::cout << "Initial Capital: $" << initialCapital << std::endl;
    std::cout << "Entry Threshold: " << entryThreshold << " sigma" << std::endl;
    std::cout << "Exit Threshold: " << exitThreshold << " sigma" << std::endl;
    std::cout << "Lookback Window: " << lookbackWindow << " days" << std::endl;
    std::cout << "Execution: " << (delayedExecution ? "T+1" : "Same day") << "\n" << std::endl;
    
    backtester.runBacktest(initialCapital, entryThreshold, exitThreshold, lookbackWindow, delayedExecution);
    
    // Export results
    std::cout << "\nExporting results to " << outputFile << std::endl;
    if (backtester.exportResults(outputFile)) {
        std::cout << "Results successfully exported" << std::endl;
    } else {
        std::cerr << "Error: Failed to export results" << std::endl;
    }
    
    return 0;
} 