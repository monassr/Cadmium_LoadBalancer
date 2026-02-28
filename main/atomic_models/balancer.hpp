#ifndef BALANCER_HPP
#define BALANCER_HPP

#include <iostream>
#include <fstream>
#include <queue>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct balancerState {
    
    bool phase;  // true = active, false = passive
    std::queue<int> job_queue;
    mutable double current_time;
    double sigma;  
    
    explicit balancerState() : phase(false), current_time(0.0), sigma(std::numeric_limits<double>::infinity()) { }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const balancerState& state) {
    out << "{phase: " << (state.phase ? "active" : "passive") << ", queue_size: " << state.job_queue.size() << "}";
    return out;
}
#endif

class balancer : public Atomic<balancerState> {
    public:
    
    //declare ports
    Port<int> balancer_in;
    Port<int> balancer_out1;
    Port<int> balancer_out2;
    Port<int> balancer_out3;
    
    // parameter: dispatch time
    double dispatch_time;
    

    mutable std::ofstream log_file;
    

    explicit balancer(const std::string& id, double disp_time = 0.5, const std::string& log_path = "simulation_results/balancer_log.txt") : Atomic<balancerState>(id, balancerState()), dispatch_time(disp_time)
    {
        
        log_file.open(log_path, std::ios::app);

        if (!log_file.is_open()) {
            std::cerr << "Warning: Could not open log file: " << log_path << std::endl;
        }

        balancer_in = addInPort<int>("balancer_in");

        
        balancer_out1 = addOutPort<int>("balancer_out1");
        balancer_out2 = addOutPort<int>("balancer_out2");
        balancer_out3 = addOutPort<int>("balancer_out3");
    }
    
    // internal transition
    void internalTransition(balancerState& state) const override {


        if (!state.job_queue.empty()) {
            state.job_queue.pop();
        }
        
        if (!state.job_queue.empty()) {

            state.phase = true;
            state.sigma = dispatch_time;  
        } else {

            state.phase = false;  
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // external transition
    void externalTransition(balancerState& state, double e) const override {

        state.current_time += e;
        
        if (state.phase) {
            state.sigma -= e;  
        }
        
        auto messages = balancer_in->getBag();
        for (const auto& msg : messages) {
            int job_id = msg;

            std::cout << state.current_time << "\tBalancer receives Job# " << job_id << " at balancer_in" << std::endl;
            if (log_file.is_open()) {
                log_file << state.current_time << "\tBalancer receives Job# "  << job_id <<" at balancer_in" << std::endl;
            }

            bool was_empty = state.job_queue.empty();
            state.job_queue.push(job_id);
            
            if (was_empty) {
                state.phase = true;  
                state.sigma = dispatch_time;  
            }
        }
    }
    
    // output function
    void output(const balancerState& state) const override {


        state.current_time += state.sigma;
        
        if (!state.job_queue.empty()) {

            int job_id = state.job_queue.front();
            
            if (job_id % 3 == 0) {

                std::cout << state.current_time << "\tBalancer sends job# " << job_id << " to server 1 at balancer_out1" << std::endl;
                if (log_file.is_open()) {
                    log_file << state.current_time << "\tBalancer sends job# " << job_id << " to server 1 at balancer_out1" << std::endl;
                }
                balancer_out1->addMessage(job_id);

            } else if (job_id % 3 == 1) {

                std::cout << state.current_time << "\tBalancer sends job# " << job_id << " to server 2 at balancer_out2" << std::endl;
                if (log_file.is_open()) {
                    log_file << state.current_time << "\tBalancer sends job# " << job_id << " to server 2 at balancer_out2" << std::endl;
                }
                balancer_out2->addMessage(job_id);

            } else {  

                std::cout << state.current_time << "\tBalancer sends job# " << job_id << " to server 3 at balancer_out3" << std::endl;
                if (log_file.is_open()) {
                    log_file << state.current_time << "\tBalancer sends job# " << job_id << " to server 3 at balancer_out3" << std::endl;
                }
                balancer_out3->addMessage(job_id);

            }
        }
    }
    
    // time_advance function
    [[nodiscard]] double timeAdvance(const balancerState& state) const override {
        return state.sigma;  
    }
    
    // destructor to close log file
    ~balancer() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
};

#endif
