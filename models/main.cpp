#include <iostream>
#include <vector> 
#include <chrono>
#include <iomanip>


#include "../core/engine.hpp"
#include "GeoBrownian.hpp"

using namespace models;
using namespace core;
using namespace std::chrono_literals; 

int main() {
    std::cout << "=== Simulation Engine Startup ===" << std::endl;

    try {
        Timepoint t0(0ns);
        Engine engine(t0);

        GeoBrownian::Parameters params{100.0, 0.05, 0.20, 0.0, 1.0};
        GeoBrownian gbm(params);

        std::cout<< "Start Price"<< gbm.get_price();

        std::cout << "[Setup] Engine initialized at t=0" << std::endl;
        
        auto horizon = t0+ std::chrono::hours(24*365);
        auto dt = std::chrono::hours(24);
        
        gbm.simulate_path(engine, dt, horizon);

        auto start_time = std::chrono::high_resolution_clock::now();
        engine.run();
        auto end_time = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration = end_time - start_time;

        std::cout<<"Final price "<< gbm.get_price()<<std::endl;
        std::cout<<"The duration was "<< duration<<std::endl;
    } catch (std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR] Simulation crashed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


