#include <iostream>
#include <format> 
#include <chrono>

#include "core/engine.hpp"
#include "bench/main_bench.cpp"

using namespace core;
using namespace std::chrono_literals; 

int main() {
    std::cout << "=== Simulation Engine Startup ===" << std::endl;

    try {
        Timepoint t0(0ns);
        Engine engine(t0);

        std::cout << "[Setup] Engine initialized at t=0" << std::endl;
        engine.schedule(t0 + 500ns, [&]() {
            std::cout << "[Event] T=500ns: Late event executed (Expected: 3rd)" << std::endl;
        });
        engine.schedule(t0 + 100ns, [&]() {
            std::cout << "[Event] T=100ns: Early event executed (Expected: 1st)" << std::endl;
        });
        engine.schedule(t0 + 300ns, [&]() {
            std::cout << "[Event] T=300ns: Sequence A (Expected: 2nd - Part 1)" << std::endl;
        });

        engine.schedule(t0 + 300ns, [&]() {
            std::cout << "[Event] T=300ns: Sequence B (Expected: 2nd - Part 2)" << std::endl;
        });
        engine.schedule(t0 + 1000ns, [&engine]() {
            std::cout << "[Event] T=1000ns: Stop command received." << std::endl;
            engine.stop();
        });
        std::cout << "[Run] Starting simulation loop..." << std::endl;
        engine.run();
        std::cout << "[Run] Simulation finished gracefully." << std::endl;
        std::cout << "[Final Time] Engine clock is now at: " << engine.now().time_since_epoch().count() << " ns" << std::endl;
    } catch (std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR] Simulation crashed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


