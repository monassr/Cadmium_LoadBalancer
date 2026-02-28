#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <fstream>
#include <queue>
#include <limits>
#include <random>
#include <cmath>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct serverState {
    
    bool phase;  // true = active, false = passive

    bool waiting;   // true = waiting for DB response, false = processing

    std::queue<int> job_queue;
    int current_job_id;
    double sigma;
    mutable double current_time;  
    
    explicit serverState() : phase(false), waiting(false), current_job_id(0), sigma(std::numeric_limits<double>::infinity()), current_time(0.0) { }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const serverState& state) {
    out << "{phase: " << (state.phase ? "active" : "passive") << ", waiting: " << (state.waiting ? "true" : "false")
    << ", queue_size: " << state.job_queue.size() << ", current_job: " << state.current_job_id << "}";
    return out;
}
#endif

class server : public Atomic<serverState> {
    public:
    
    // Declare input and output ports
    Port<int> server_in;      
    Port<int> server_in_db;     
    Port<int> server_out1;  
    Port<int> server_out2;      
    
    int server_id;           
    mutable double processing_time;  
    mutable int pid_sent;   
    mutable std::ofstream log_file;
    mutable std::mt19937 rng;                          // random number generator
    mutable std::exponential_distribution<double> dist; // exponential distribution
    

    double getProcessingTime() const {
        return fabs(dist(rng));
    }
    
    explicit server(const std::string& id, int sid, double mean, const std::string& log_path = "")  : Atomic<serverState>(id, serverState()),  server_id(sid),  processing_time(0), pid_sent(0), rng(std::random_device{}()), dist(1.0 / mean) 
    {  

        server_in = addInPort<int>("server_in");
        server_in_db = addInPort<int>("server_in_db");
        
 
        server_out1 = addOutPort<int>("server_out1");
        server_out2 = addOutPort<int>("server_out2");
        
   
        std::string path = log_path.empty() ? "simulation_results/server_log.txt" : log_path;

        log_file.open(path, std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "Warning: Could not open log file: " << path << std::endl;
        }
    }

    // internal transition
    void internalTransition(serverState& state) const override {

        if (!state.waiting && !state.job_queue.empty()) {
            state.job_queue.pop();
        }
        
        state.waiting = !state.waiting;
        
        if (!state.waiting && !state.job_queue.empty()) {
            state.current_job_id = state.job_queue.front();
            std::cout << state.current_time << "\tServer " << server_id << " starts processing job# " << state.current_job_id << std::endl;
            if (log_file.is_open()) {
                log_file << state.current_time << "\tServer " << server_id << " starts processing job# " << state.current_job_id << std::endl;
            }
            state.phase = true;
            processing_time = getProcessingTime();
            state.sigma = processing_time;
            
        } else {

            state.phase = false;
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // external transition
    void externalTransition(serverState& state, double e) const override {
        
        state.current_time += e;
        
        if (state.phase) {
            state.sigma -= e;
        }
        
        auto in_messages = server_in->getBag();
        auto in_db_messages = server_in_db->getBag();
        
        if (!in_messages.empty()) {
            int job = in_messages.back();
            state.job_queue.push(job);
            
            std::cout << state.current_time << "\tServer " << server_id << " receives job# " << job << " at server_in1" << std::endl;
            if (log_file.is_open()) {
                log_file << state.current_time << "\tServer " << server_id << " receives job# " << job << " at server_in1" << std::endl;
            }
            
            if (state.job_queue.size() == 1 && !state.waiting) {
                state.current_job_id = job;
                std::cout << state.current_time << "\tServer " << server_id << " starts processing job# " << job << std::endl;
                if (log_file.is_open()) {
                    log_file << state.current_time << "\tServer " << server_id << " starts processing job# " << job << std::endl;
                }
                state.phase = true;  
                processing_time = getProcessingTime();
                state.sigma = processing_time;
            }
        }
        
        if (!in_db_messages.empty() && state.waiting) {
            std::cout << state.current_time << "\tServer " << server_id << " receives DB acknowledgment for job# " << pid_sent << " at server_in_db" << std::endl;
            if (log_file.is_open()) {
                log_file << state.current_time << "\tServer " << server_id << " receives DB acknowledgment for job# " << pid_sent << " at server_in_db" << std::endl;
            }
            state.phase = true;  
            state.sigma = 0.0;   
        }
    }


    // output function
    void output(const serverState& state) const override {

        state.current_time += state.sigma;
        
        if (state.waiting) {
            
            std::cout << state.current_time << "\tServer " << server_id << " finishes job# " << pid_sent << " at server_out1" << std::endl;
            if (log_file.is_open()) {
                log_file << state.current_time << "\tServer " << server_id << " finishes job# " << pid_sent << " at server_out1" << std::endl;
            }
            server_out1->addMessage(pid_sent);
        } else {
           
            pid_sent = state.job_queue.front();
            std::cout << state.current_time << "\tServer " << server_id << " sends job# " << pid_sent << " to database server at server_out2" << std::endl;
            if (log_file.is_open()) {
                log_file << state.current_time << "\tServer " << server_id << " sends job# " << pid_sent << " to database server at server_out2" << std::endl;
            }
            server_out2->addMessage(server_id);  
        }
    }
    
    // time_advance function
    [[nodiscard]] double timeAdvance(const serverState& state) const override {
        if (!state.phase) {
            return std::numeric_limits<double>::infinity();  // passive
        }
        return state.sigma;
    }
    
    // destructor to close log file
    ~server() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
};

#endif
