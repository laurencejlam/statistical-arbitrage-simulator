#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../include/MarketData.h"
#include "../include/AssetPair.h"
#include "../include/Utilities.h"
#include "../include/Backtester.h"

#include <memory>
#include <vector>
#include <cmath>

// Note: Before running these tests, you need to download catch.hpp to the tests directory
// wget https://github.com/catchorg/Catch2/releases/download/v2.13.6/catch.hpp

TEST_CASE("Utilities functions", "[utilities]") {
    SECTION("Basic statistical functions") {
        std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
        
        REQUIRE(Utilities::mean(data) == Approx(3.0));
        REQUIRE(Utilities::standardDeviation(data) == Approx(1.58114).epsilon(0.01));
    }
    
    SECTION("Rolling calculations") {
        std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
        size_t window = 3;
        
        auto rolling_mean = Utilities::rollingMean(data, window);
        REQUIRE(rolling_mean[2] == Approx(2.0));
        REQUIRE(rolling_mean[5] == Approx(5.0));
        REQUIRE(rolling_mean[9] == Approx(9.0));
        
        auto rolling_std = Utilities::rollingStdDev(data, window);
        REQUIRE(rolling_std[2] == Approx(1.0));
        REQUIRE(rolling_std[5] == Approx(1.0));
    }
    
    SECTION("Linear regression") {
        std::vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};
        std::vector<double> y = {2.0, 4.0, 6.0, 8.0, 10.0};
        
        auto regression = Utilities::linearRegression(x, y);
        
        REQUIRE(regression.alpha == Approx(0.0));
        REQUIRE(regression.beta == Approx(2.0));
        REQUIRE(regression.rsquared == Approx(1.0));
    }
}

TEST_CASE("Market Data loading", "[market_data]") {
    // This test requires a sample data file
    // Create a small test data file
    std::ofstream test_file("test_data.csv");
    test_file << "Date,A,B,C\n";
    test_file << "2020-01-01,100,200,300\n";
    test_file << "2020-01-02,101,202,305\n";
    test_file << "2020-01-03,102,204,310\n";
    test_file.close();
    
    MarketData market_data("test_data.csv");
    
    REQUIRE(market_data.getDataSize() == 3);
    REQUIRE(market_data.getAvailableSymbols().size() == 3);
    
    auto prices_a = market_data.getPriceSeries("A");
    REQUIRE(prices_a.has_value());
    REQUIRE(prices_a->size() == 3);
    REQUIRE((*prices_a)[0] == Approx(100.0));
    REQUIRE((*prices_a)[2] == Approx(102.0));
    
    // Clean up
    std::remove("test_data.csv");
}

TEST_CASE("Asset Pair functionality", "[asset_pair]") {
    std::vector<double> prices_a = {100.0, 101.0, 102.0, 101.5, 101.0, 100.5, 101.0, 102.0, 103.0, 102.5};
    std::vector<double> prices_b = {200.0, 202.0, 204.0, 203.0, 202.0, 201.0, 202.0, 204.0, 206.0, 205.0};
    
    AssetPair pair("A", "B", prices_a, prices_b);
    
    SECTION("Basic properties") {
        REQUIRE(pair.getSymbolA() == "A");
        REQUIRE(pair.getSymbolB() == "B");
        REQUIRE(pair.getPricesA().size() == 10);
        REQUIRE(pair.getPricesB().size() == 10);
    }
    
    SECTION("Cointegration") {
        // These are perfectly cointegrated with beta = 0.5
        REQUIRE(pair.testCointegration());
        REQUIRE(pair.getCointegrationBeta() == Approx(0.5).epsilon(0.01));
    }
    
    SECTION("Z-scores") {
        auto zscores = pair.getZScores(3);
        REQUIRE(zscores.size() == 10);
    }
    
    SECTION("Signal generation") {
        auto signals = pair.generateSignals(1.0, 0.0, 3);
        REQUIRE(signals.size() == 10);
    }
}

TEST_CASE("Backtester execution", "[backtester]") {
    // Create a small test data file
    std::ofstream test_file("backtest_data.csv");
    test_file << "Date,A1,B1,A2,B2\n";
    for (int i = 0; i < 100; ++i) {
        double a1 = 100.0 + i * 0.1 + sin(i * 0.3) * 5.0;
        double b1 = a1 * 2.0 + sin(i * 0.3) * 2.0;
        double a2 = 50.0 + i * 0.2 + sin(i * 0.3) * 3.0;
        double b2 = a2 * 0.5 + sin(i * 0.3) * 1.0;
        test_file << "2020-01-" << (i + 1) << "," << a1 << "," << b1 << "," << a2 << "," << b2 << "\n";
    }
    test_file.close();
    
    auto market_data = std::make_shared<MarketData>("backtest_data.csv");
    Backtester backtester(market_data);
    
    // Add pairs
    backtester.addPair("A1", "B1");
    backtester.addPair("A2", "B2");
    
    // Run backtest
    backtester.runBacktest(100000.0, 1.5, 0.0, 20, true);
    
    // Check results
    auto metrics = backtester.getPerformanceMetrics();
    REQUIRE(backtester.getPortfolioValues().size() == 100);
    
    // Check that metrics are calculated (we don't know exact values)
    REQUIRE_FALSE(std::isnan(metrics.totalReturn));
    REQUIRE_FALSE(std::isnan(metrics.sharpeRatio));
    REQUIRE_FALSE(std::isnan(metrics.maxDrawdown));
    
    // Clean up
    std::remove("backtest_data.csv");
} 