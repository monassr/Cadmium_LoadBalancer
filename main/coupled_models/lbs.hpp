#ifndef LBS_HPP
#define LBS_HPP

#include <memory>
#include <fstream>
#include "cadmium/modeling/devs/coupled.hpp"
#include "../atomic_models/balancer.hpp"
#include "../atomic_models/server.hpp"
#include "../atomic_models/dbserver.hpp"

using namespace cadmium;

struct LBS : public Coupled {

    std::shared_ptr<cadmium::PortInterface> in;
    std::shared_ptr<cadmium::PortInterface> out;
    
    LBS(const std::string& id, const std::string& log_path = "simulation_results/lbs_log.txt") : Coupled(id) {

        
        // create external input and output ports 
        in = addInPort<int>("in");
        out = addOutPort<int>("out");

        // create atomic components

        // model name, dipatch time, log path
        auto bal = addComponent<balancer>("balancer", 1, log_path);  

        // model name, server id, mean processing time, log path
        auto srv1 = addComponent<server>("server1", 1, 0.5, log_path);  
        auto srv2 = addComponent<server>("server2", 2, 0.5, log_path);  
        auto srv3 = addComponent<server>("server3", 3, 0.5, log_path); 
        
        // model name, db processing time, log path
        auto db = addComponent<dbserver>("db_server", 1, log_path);  

        // external input couplings
        addCoupling(in, bal->balancer_in);
        
        // external Output Couplings
        addCoupling(srv1->server_out1, out);
        addCoupling(srv2->server_out1, out);
        addCoupling(srv3->server_out1, out);
        
        // internal Couplings
        addCoupling(bal->balancer_out1, srv1->server_in);
        addCoupling(bal->balancer_out2, srv2->server_in);
        addCoupling(bal->balancer_out3, srv3->server_in);

        addCoupling(srv1->server_out2, db->dbserver_in);
        addCoupling(srv2->server_out2, db->dbserver_in);
        addCoupling(srv3->server_out2, db->dbserver_in);
        
        addCoupling(db->dbserver_out1, srv1->server_in_db);
        addCoupling(db->dbserver_out2, srv2->server_in_db);
        addCoupling(db->dbserver_out3, srv3->server_in_db);
    }
};

#endif
