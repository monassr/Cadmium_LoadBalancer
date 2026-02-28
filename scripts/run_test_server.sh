#!/bin/bash
# Build and run the Server test

cd "$(dirname "$0")/." || exit
cd ..

echo "================================"
echo "Building Server Test"
echo "================================"

if [ -d "build" ]; then rm -Rf build; fi
mkdir -p build && cd build || exit
cmake .. -DSIM=ON > /dev/null 2>&1
make test_server

echo ""
echo "================================"
echo "Running Server Test"
echo "================================"
cd ..
rm -f simulation_results/server*_log.txt
rm -f simulation_results/server_output.csv
./bin/test_server
echo ""
echo "Readable output saved to: simulation_results/server1_log.txt"
echo "Cadmium logger output saved to: simulation_results/server_output.csv"