cmake_minimum_required(VERSION 3.10)
project(StatArbSimulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(include)

# Find Eigen package if available
find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    include_directories(${EIGEN3_INCLUDE_DIR})
    add_definitions(-DUSE_EIGEN)
    message(STATUS "Eigen found at ${EIGEN3_INCLUDE_DIR}")
else()
    message(STATUS "Eigen not found, will use standard library instead")
endif()

# Add the executable
add_executable(StatArbSimulator 
    src/main.cpp
    src/MarketData.cpp
    src/AssetPair.cpp
    src/Backtester.cpp
    src/Utilities.cpp
)

# Add test executable if tests are built
option(BUILD_TESTS "Build the tests" ON)
if(BUILD_TESTS)
    add_executable(RunTests
        tests/test_main.cpp
        tests/test_marketdata.cpp
        tests/test_assetpair.cpp
        tests/test_backtester.cpp
        tests/test_cointegration.cpp
        # Link to implementation files
        src/MarketData.cpp
        src/AssetPair.cpp
        src/Backtester.cpp
        src/Utilities.cpp
    )
endif()

# Install target
install(TARGETS StatArbSimulator DESTINATION bin) 