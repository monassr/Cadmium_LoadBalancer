#!/bin/bash
# Build and run the generator test

cd "$(dirname "$0")/." || exit
cd ..

echo "================================"
echo "Building Generator Test"
echo "================================"

if [ -d "build" ]; then rm -Rf build; fi
mkdir -p build && cd build || exit
cmake .. -DSIM=ON > /dev/null 2>&1
make test_generator

echo ""
echo "================================"
echo "Running Generator Test"
echo "================================"
cd ..
rm -f simulation_results/generator_log.txt
rm -f simulation_results/generator_output.csv
./bin/test_generator
echo ""
echo "Readable output saved to: simulation_results/generator_log.txt"
echo "Cadmium logger output saved to: simulation_results/generator_output.csv"
