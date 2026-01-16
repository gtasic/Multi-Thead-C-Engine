//C'est le main brute
//sim_engine.exe est le fichier exécutable associé avec compilateur en mode débug
//benchmark_opti.exe est le fichier exécutable associé avec compilateur sans mode débug

#include <iostream>
#include <vector>
#include <chrono> 
#include <iomanip>
#include <array>

#include "../core/Engine.hpp"

using namespace core;

const int N_EVMT = 1000000;


int main(){

    std::cout<< "===Performance Benchmark===" << std::endl;
    std::cout<< "Number of events"<< N_EVMT << std::endl;

    Timepoint t0(std::chrono::nanoseconds(0));
    Engine engine(t0);

    std::cout<<"Injection of tasks"<<std::endl;

    auto empty_task = [](){
        asm volatile("" ::: "memory");
    };

    auto start_inject = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N_EVMT; i++){
        engine.schedule(t0 + std::chrono::nanoseconds(i), empty_task);
    }
    auto end_inject = std::chrono::high_resolution_clock::now();


    std::cout<< "===Execution part starting===" << std::endl;
    auto start_run  = std::chrono::high_resolution_clock::now();
    engine.run();
    auto end_run  = std::chrono::high_resolution_clock::now();


    std::chrono::duration<double> duration_inject = end_inject - start_inject;
    std::chrono::duration<double> duration_run = end_run - start_run;

    double event_par_seconde = N_EVMT/duration_run.count();

    double latency_ns = (duration_run.count() * 1e9) / N_EVMT;

    std::cout << "\n=== Results ===" << std::endl;
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Injection Time : " << duration_inject.count() << " s" << std::endl;
    std::cout << "Execution Time : " << duration_run.count() << " s" << std::endl;
    
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Throughput     : " << std::setprecision(0) << event_par_seconde << " events/sec" << std::endl;
    std::cout << "Avg Latency    : " << std::setprecision(2) << latency_ns << " ns/event" << std::endl;

    if (event_par_seconde > 5'000'000) {
        std::cout << "[SUCCESS] Engine is fast (> 5M/s)" << std::endl;
    } else {
        std::cout << "[INFO] Engine needs optimization (Memory Allocators missing)" << std::endl;
    }

    return 0;    
    
}
  