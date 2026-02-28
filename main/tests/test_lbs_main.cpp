/*
Test main file for the LBS coupled model.
*/

#include <limits>
#include "cadmium/lib/iestream.hpp"
#include "../coupled_models/lbs.hpp"



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


struct test_lbs_coupled : public Coupled {
    test_lbs_coupled(const std::string& id) : Coupled(id) {
		
        // create IEStream component to read int from CSV file
        auto job_stream = addComponent<lib::IEStream<int>>("In", "<ABSOLUTE_PATH>/main/test_inputs/Input_In_LBS_Testing.csv");
        auto lbs = addComponent<LBS>("LBS", "simulation_results/lbs_log.txt");  
        
        // connect IEStream output to LBS input
        addCoupling(job_stream->out, lbs->in);
    }
};

extern "C" {
	#ifdef ESP_PLATFORM
		void app_main()
	#else
		int main()
	#endif
	{
		auto model = std::make_shared<test_lbs_coupled>("test_lbs");
		
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
		rootCoordinator.setLogger<CSVLogger>("simulation_results/lbs_output.csv", ";");
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
