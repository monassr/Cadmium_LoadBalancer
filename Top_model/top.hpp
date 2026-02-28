#ifndef TOP_HPP
#define TOP_HPP

#include <memory>
#include <fstream>
#include "cadmium/modeling/devs/coupled.hpp"
#include "../atomic_models/generator.hpp"
#include "lbs.hpp"

using namespace cadmium;

struct Top_coupled: public Coupled {
 
    std::shared_ptr<cadmium::PortInterface> out;
    
    Top_coupled(const std::string& id, const std::string& log_path = "simulation_results/top_log.txt") : Coupled(id) {

           
        out = addOutPort<int>("out");

        auto gen = addComponent<generator>("generator", 0.3, log_path);  
        auto lbs = addComponent<LBS>("LBS", log_path);              
        
        // external output coupling
        addCoupling(lbs->out, out);
        
        // internal coupling
        addCoupling(gen->generator_out1, lbs->in);
    }
};

#endif
