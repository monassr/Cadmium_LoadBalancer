#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <iostream>
#include <fstream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct generatorState {
    mutable int job_id;
    double sigma;
    mutable double current_time;
    
    explicit generatorState(double output_rate = 0.1) : job_id(1), sigma(output_rate), current_time(0.0) { }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const generatorState& state) {
    out << "{job_id: " << state.job_id << "}";
    return out;
}
#endif

class generator : public Atomic<generatorState> {
    public:
    
    Port<int> generator_out1;
    
    double output_rate;
    
    mutable std::ofstream log_file;
    
    explicit generator(const std::string& id, double rate = 0.1, const std::string& log_path = "simulation_results/generator_log.txt") : Atomic<generatorState>(id, generatorState(rate)), output_rate(rate)
    {

        generator_out1 = addOutPort<int>("generator_out1");
        
        log_file.open(log_path, std::ios::app);

        if (!log_file.is_open()) {
            std::cerr << "Warning: Could not open log file: " << log_path << std::endl;
        }
    }
    
    // internal transition
    void internalTransition(generatorState& state) const override {
        state.job_id = state.job_id + 1;
    }
    
    // external transition
    void externalTransition(generatorState& state, double e) const override {
        // No external inputs for this model
    }
    
    // output function
    void output(const generatorState& state) const override {

        state.current_time += state.sigma;
        int job_id = state.job_id;
        std::cout << state.current_time << "\tGenerator outputs Job# " << job_id << " at generator_out1" << std::endl;
        if (log_file.is_open()) {
            log_file << state.current_time << "\tGenerator outputs Job# " << job_id << " at generator_out1" << std::endl;
        }
        generator_out1->addMessage(job_id);
    }
    
    //  time_advance function
    [[nodiscard]] double timeAdvance(const generatorState& state) const override {
        return state.sigma;  
    }
    
    // destructor to close log file
    ~generator() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
};

#endif
