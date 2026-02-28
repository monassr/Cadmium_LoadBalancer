# Load Balancer — Cadmium DEVS Simulation

A load balancer system modeled using the DEVS formalism and implemented with the [Cadmium v2](https://github.com/Sasisekhar/cadmium_v2) simulation framework in C++.

## Project Structure

```
atomic_models [This folder contains the atomic DEVS models implemented in Cadmium]
	generator.hpp
	balancer.hpp
	server.hpp
	dbserver.hpp
bin [This folder will be created automatically the first time you compile the project.
     It will contain all the executables]
build [This folder will be created automatically the first time you compile the project.
       It will contain all the build files generated during compilation]
coupled_models [This folder contains the coupled DEVS models]
	lbs.hpp
scripts [This folder contains shell scripts to build and run each test]
	run_test_generator.sh
	run_test_balancer.sh
	run_test_server.sh
	run_test_db_server.sh
	run_test_lbs.sh
	run_test_top.sh
simulation_results [This folder will be created automatically the first time you compile the project.
                    It will store the outputs from your simulations and tests]
test_inputs [This folder contains all the CSV input data to run the model tests]
	Input_In_Balancer_Testing.csv
	Input_In_Server_Testing.csv
	Input_Indb_Server_Testing.csv
	Input_In_DBServer_Testing.csv
	Input_In_LBS_Testing.csv
tests [This folder contains the unit tests for the atomic and coupled models]
	test_generator_main.cpp
	test_balancer_main.cpp
	test_server_main.cpp
	test_dbserver_main.cpp
	test_lbs_main.cpp
	test_top_main.cpp
Top_model [This folder contains the Top-level coupled model]
	top.hpp
```

## Prerequisites

- [Cadmium v2](https://github.com/Sasisekhar/cadmium_v2)

## Important: Absolute Paths for Test Inputs

Several test files use Cadmium's `IEStream` to read input data from CSV files. `IEStream` requires absolute paths. Before building, you must update the placeholder `<ABSOLUTE_PATH>` in the following files to match your system:

- `main/tests/test_balancer_main.cpp`
- `main/tests/test_server_main.cpp`
- `main/tests/test_dbserver_main.cpp`
- `main/tests/test_lbs_main.cpp`

For example, if your project is at `/home/user/LoadBalancer`, replace:
```
<ABSOLUTE_PATH>/main/test_inputs/Input_In_Balancer_Testing.csv
```
with:
```
/home/user/LoadBalancer/main/test_inputs/Input_In_Balancer_Testing.csv
```


## Building

Each run script handles the full build-and-run cycle automatically. If you want to build manually:

```bash
mkdir -p build && cd build
cmake .. -DSIM=ON
make <target>
```

Available build targets:
| Target | Description |
|---|---|
| `test_generator` | Generator atomic model test |
| `test_balancer` | Balancer atomic model test |
| `test_server` | Server atomic model test |
| `test_dbserver` | DB Server atomic model test |
| `test_lbs` | LBS coupled model test |
| `test_top` | Full system (Top) test |

Binaries are placed in the `bin/` directory.

## Running Tests

The simplest way to run any test is through the provided shell scripts in `scripts/`. to ensure scripts are executable run the below code only once before running the scripts.

```bash
chmod +x scripts/*.sh
```

### Individual Model Tests

**Generator** — generates a job every second for a total of 4 seconds:
```bash
./scripts/run_test_generator.sh
```

**Balancer** — dispatches jobs following round-robin scheduling:
```bash
./scripts/run_test_balancer.sh
```

**Server** — processes 3 jobs:
```bash
./scripts/run_test_server.sh
```

**DB Server** — receives jobs, processes them, sends acknowledgments back:
```bash
./scripts/run_test_db_server.sh
```

### Coupled Model Tests

**LBS** — tests the load balance system (balancer + 3 servers + dbserver) for 1 hour:
```bash
./scripts/run_test_lbs.sh
```

**Top** — tests the complete system generator integrated into the LBS (generator + balancer + 3 servers + dbserver) for 1 hour:
```bash
./scripts/run_test_top.sh
```

## Simulation Output

Each test produces two output files in `simulation_results/`:

| File | Format | Description |
|---|---|---|
| `*_log.txt` | Tab-separated text | Human-readable log with timestamps and event descriptions |
| `*_output.csv` | Semicolon-delimited CSV | Cadmium's built-in logger output with columns: `time;model_id;model_name;port_name;data` |

Both files contain equivalent information. The `.txt` logs are easier to read.
