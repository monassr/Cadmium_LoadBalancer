#!/bin/bash
# Build and run the balancer test

cd "$(dirname "$0")/." || exit
cd ..

echo "================================"
echo "Building Balancer Test"
echo "================================"

if [ -d "build" ]; then rm -Rf build; fi
mkdir -p build && cd build || exit
cmake .. -DSIM=ON > /dev/null 2>&1
make test_balancer

echo ""
echo "================================"
echo "Running Balancer Test"
echo "================================"
cd ..
rm -f simulation_results/balancer_log.txt
rm -f simulation_results/balancer_output.csv
./bin/test_balancer
echo ""
echo "Readable output saved to: simulation_results/balancer_log.txt"
echo "Cadmium logger output saved to: simulation_results/balancer_output.csv"
