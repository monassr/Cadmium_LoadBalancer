#!/bin/bash
# Build and run the TOP (Top-level system) test

cd "$(dirname "$0")/." || exit
cd ..

echo "================================"
echo "Building TOP Test"
echo "================================"

if [ -d "build" ]; then rm -Rf build; fi
mkdir -p build && cd build || exit
cmake .. -DSIM=ON > /dev/null 2>&1
make test_top

echo ""
echo "================================"
echo "Running TOP Test"
echo "================================"
cd ..
rm -f simulation_results/top_log.txt
rm -f simulation_results/top_output.csv
./bin/test_top
echo ""
echo "Readable output saved to: simulation_results/top_log.txt"
echo "Cadmium logger output saved to: simulation_results/top_output.csv"
