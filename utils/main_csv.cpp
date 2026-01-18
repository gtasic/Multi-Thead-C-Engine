#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <chrono>

#include "../core/Engine.hpp"
#include "../models/GeoBrownian.hpp"
#include "../memory/ThreadRessource.hpp" 
#include "CsvWriter.hpp" 

using namespace core;
using namespace models;
using namespace memory;

const int N_TOTAL_PATHS = 10000; 

int main() {
    std::cout << "=== Monte Carlo Simulation with CSV Export ===" << std::endl;

    SafePoolThread<50 * 1024 * 1024> global_pool;
    utils::CsvWriter writer("simulation_data.csv"); 

    const int num_threads = std::thread::hardware_concurrency();
    const int paths_per_thread = N_TOTAL_PATHS / num_threads;

    std::vector<std::future<double>> futures;

    for (int t = 0; t < num_threads; ++t) {
        
        futures.push_back(std::async(std::launch::async, [&, t]() {
            
            Timepoint t0(std::chrono::nanoseconds(0));
            Engine engine(t0, &global_pool);

            GeoBrownian::Parameters params { 100.0, 0.05, 0.20, 0.0, 1.0 };
            GeoBrownian gbm(params);

            auto horizon = t0 + std::chrono::hours(365 * 24); 
            auto dt = std::chrono::hours(24); 

            double local_payoff_sum = 0.0;
            
            bool is_logger_thread = (t == 0);
            int max_paths_to_log = 50;

            for (int i = 0; i < paths_per_thread; ++i) {
                
                if (is_logger_thread && i < max_paths_to_log) {
                    
                    gbm.reset_price(100.0);
                    Timepoint current_time = t0;

                    writer.log_row(0.0, i, 100.0);

                    while (current_time < horizon) {
                        gbm.simulate_path(engine, dt, current_time + dt + std::chrono::microseconds(10));
                        engine.run(current_time + dt + std::chrono::microseconds(10));
                        
                        current_time += dt;

                        double days = std::chrono::duration<double>(current_time - t0).count() / (24.0 * 3600.0);
                        writer.log_row(days, i, gbm.get_price());
                    }

                } else {
                    gbm.reset_price(100.0);
                    gbm.simulate_path(engine, dt, horizon);
                    engine.run(horizon);
                }

                double final_price = gbm.get_price();
                local_payoff_sum += std::max(final_price - 100.0, 0.0);

                engine.reset(t0);
                global_pool.release_current_thread();
            }

            return local_payoff_sum;
        }));
    }

    double total_payoff = 0.0;
    for (auto& f : futures) {
        total_payoff += f.get();
    }

    double average_price = total_payoff / N_TOTAL_PATHS;
    double option_price = average_price * std::exp(-0.05 * 1.0);

    std::cout << "Simulation finished." << std::endl;
    std::cout << "Data exported to 'simulation_data.csv'" << std::endl;
    std::cout << "Call Option Price: " << option_price << std::endl;

    return 0;
}