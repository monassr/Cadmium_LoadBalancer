#ifndef DBSERVER_HPP
#define DBSERVER_HPP

#include <iostream>
#include <fstream>
#include <queue>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// DBServer state structure
struct dbserverState {
    bool phase;  // true = active, false = passive
    double sigma;
    std::queue<int> job_queue;  
    mutable double current_time;
    mutable int jobs_done;

    explicit dbserverState() : phase(false), sigma(std::numeric_limits<double>::infinity()), current_time(0.0), jobs_done(0) {}
};


#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream& os, const dbserverState& state) {
    os << "{phase: " << (state.phase ? "active" : "passive") 
       << ", queue_size: " << state.job_queue.size() << ", jobs_done: " << state.jobs_done << "}";
    return os;
}
#endif

class dbserver : public Atomic<dbserverState> {
public:

    Port<int> dbserver_in;      
    Port<int> dbserver_out1;   
    Port<int> dbserver_out2;    
    Port<int> dbserver_out3;    

private:
    double dbprocessing_time;
    mutable std::ofstream log_file;  

public:

    explicit dbserver(const std::string& id, double proc_time, const std::string& log_path = "simulation_results/dbserver_log.txt"): Atomic<dbserverState>(id, dbserverState()), dbprocessing_time(proc_time) {
        
        dbserver_in = addInPort<int>("dbserver_in");
        
       
        dbserver_out1 = addOutPort<int>("dbserver_out1");
        dbserver_out2 = addOutPort<int>("dbserver_out2");
        dbserver_out3 = addOutPort<int>("dbserver_out3");
        
      
        log_file.open(log_path, std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "Warning: Could not open log file: " << log_path << std::endl;
        }
    }

    // internal transition
    void internalTransition(dbserverState& state) const override {

        if (!state.job_queue.empty()) {
            state.job_queue.pop();
        }

   
        if (!state.job_queue.empty()) {
            state.phase = true;  
            state.sigma = dbprocessing_time;
        } else {
            state.phase = false;  
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    // external transition
    void externalTransition(dbserverState& state, double e) const override {

        state.current_time += e;
        
        if(state.phase) {
            state.sigma -= e;  
        }

        auto messages = dbserver_in->getBag();

        if (!messages.empty()) {

            int server_id = messages.back();

            state.job_queue.push(server_id);

            std::cout << state.current_time << "\tDBServer receives job from server#" << server_id << " at dbserver_in1" << std::endl;
            if (log_file.is_open()) {
                log_file << state.current_time << "\tDBServer receives job from server#" << server_id << " at dbserver_in1" << std::endl;
            }

            if (state.job_queue.size() == 1) {
                state.phase = true;  
                state.sigma = dbprocessing_time;
            }
        }
    }

    // output function
    void output(const dbserverState& state) const override {
     
        state.current_time += state.sigma;
        
        if (!state.job_queue.empty()) {

            int server_id = state.job_queue.front();

            state.jobs_done++;
            
            if (server_id == 1) {
                dbserver_out1->addMessage(server_id);
                std::cout << state.current_time << "\tDBServer sends job back to server#" << server_id << " at dbserver_out1" << std::endl;
                std::cout << state.current_time << "\tJobs done by DB Server: " << state.jobs_done << std::endl;
                if (log_file.is_open()) {
                    log_file << state.current_time << "\tDBServer sends job back to server#" << server_id << " at dbserver_out1" << std::endl;
                    log_file << state.current_time << "\tJobs done by DB Server: " << state.jobs_done << std::endl;
                }
            } else if (server_id == 2) {
                dbserver_out2->addMessage(server_id);
                std::cout << state.current_time << "\tDBServer sends job back to server#" << server_id << " at dbserver_out2" << std::endl;
                std::cout << state.current_time << "\tJobs done by DB Server: " << state.jobs_done << std::endl;
                if (log_file.is_open()) {
                    log_file << state.current_time << "\tDBServer sends job back to server#" << server_id << " at dbserver_out2" << std::endl;
                    log_file << state.current_time << "\tJobs done by DB Server: " << state.jobs_done << std::endl;
                }
            } else if (server_id == 3) {
                dbserver_out3->addMessage(server_id);
                std::cout << state.current_time << "\tDBServer sends job back to server#" << server_id << " at dbserver_out3" << std::endl;
                std::cout << state.current_time << "\tJobs done by DB Server: " << state.jobs_done << std::endl;
                if (log_file.is_open()) {
                    log_file << state.current_time << "\tDBServer sends job back to server#" << server_id << " at dbserver_out3" << std::endl;
                    log_file << state.current_time << "\tJobs done by DB Server: " << state.jobs_done << std::endl;
                }
            }
             
        }
    }

    // // Confluence transition: handles simultaneous internal and external events
    // void confluenceTransition(dbserverState& state, double e) const {
    //     internalTransition(state);       
    //     externalTransition(state, 0.0);   
    // }

    // time_advance function
    [[nodiscard]] double timeAdvance(const dbserverState& state) const override {
        if (!state.phase) {
            return std::numeric_limits<double>::infinity();  
        }
        return state.sigma;
    }
    
    // destructor to close log file
    ~dbserver() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
};

#endif
