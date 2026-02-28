/*
Test main file for the dbserver atomic model
*/

#include <limits>
#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"
#include "../atomic_models/dbserver.hpp"



#ifdef SIM_TIME
	#include "cadmium/simulation/root_coordinator.hpp"
#else
	#include "cadmium/simulation/rt_root_coordinator.hpp"
	#ifdef ESP_PLATFORM
		#include <cadmium/simulation/rt_clock/ESPclock.hpp>
	#else
		#include <cadmium/simulation/rt_clock/chrono.hpp>
	#endif
#endif

#ifndef NO_LOGGING
	#include "cadmium/simulation/logger/stdout.hpp"
	#include "cadmium/simulation/logger/csv.hpp"
#endif

using namespace cadmium;

struct test_dbserver_coupled : public Coupled {
    test_dbserver_coupled(const std::string& id) : Coupled(id) {

        // create IEStream component to read server_ids from CSV file
        auto job_stream = addComponent<lib::IEStream<int>>("job_stream", "<ABSOLUTE_PATH>/main/test_inputs/Input_In_DBServer_Testing.csv");
        
        auto dbs = addComponent<dbserver>("db_server", 0.5);
        
		// connect IEStream directly to dbserver input
        addCoupling(job_stream->out, dbs->dbserver_in);
    }
};

extern "C" {
	#ifdef ESP_PLATFORM
		void app_main()
	#else
		int main()
	#endif
	{
		auto model = std::make_shared<test_dbserver_coupled>("test_dbserver");
		
		#ifdef SIM_TIME
			auto rootCoordinator = cadmium::RootCoordinator(model);
		#else
			#ifdef ESP_PLATFORM
				cadmium::ESPclock clock;
				auto rootCoordinator = cadmium::RealTimeRootCoordinator<cadmium::ESPclock<double>>(model, clock);
			#else
				cadmium::ChronoClock clock;
				auto rootCoordinator = cadmium::RealTimeRootCoordinator<cadmium::ChronoClock<std::chrono::steady_clock>>(model, clock);
			#endif
		#endif

		#ifndef NO_LOGGING
			rootCoordinator.setLogger<STDOUTLogger>(";");
			rootCoordinator.setLogger<CSVLogger>("simulation_results/dbserver_output.csv", ";");
		#endif

		rootCoordinator.start();
		
		#ifdef ESP_PLATFORM
			rootCoordinator.simulate(std::numeric_limits<double>::infinity());
		#else
			rootCoordinator.simulate(std::numeric_limits<double>::infinity());
		#endif
		
		rootCoordinator.stop();	

		#ifndef ESP_PLATFORM
			return 0;
		#endif
	}
}
