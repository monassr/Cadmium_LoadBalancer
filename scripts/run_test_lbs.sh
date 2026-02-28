#!/bin/bash
# Build and run the LBS (Load Balance System) test

cd "$(dirname "$0")/." || exit
cd ..

echo "================================"
echo "Building LBS Test"
echo "================================"

if [ -d "build" ]; then rm -Rf build; fi
mkdir -p build && cd build || exit
cmake .. -DSIM=ON > /dev/null 2>&1
make test_lbs

echo ""
echo "================================"
echo "Running LBS Test"
echo "================================"
cd ..
rm -f simulation_results/lbs_log.txt
rm -f simulation_results/lbs_output.csv
./bin/test_lbs
echo ""
echo "Readable output saved to: simulation_results/lbs_log.txt"
echo "Cadmium logger output saved to: simulation_results/lbs_output.csv"
