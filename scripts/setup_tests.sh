#!/bin/bash
# Setup script for downloading Catch2 testing framework

echo "Downloading Catch2 header file..."
cd ../tests
wget -O catch.hpp https://github.com/catchorg/Catch2/releases/download/v2.13.6/catch.hpp

if [ $? -eq 0 ]; then
    echo "Download successful."
    echo "Catch2 is now ready to use for testing."
else
    echo "Download failed. Please check your internet connection and try again."
    exit 1
fi 