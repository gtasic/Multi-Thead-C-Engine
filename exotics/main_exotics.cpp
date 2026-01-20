#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <chrono>

#include "../core/Engine.hpp"
#include "../models/Heston.hpp"
#include "../memory/ThreadRessource.hpp" 
#include "../utils/CsvWriter.hpp" 

using namespace core;
using namespace models;
using namespace memory;

const int N_TOTAL_PATHS = 100000; 

int main() {
    std::cout << "=== Monte Carlo Simulation with CSV Export ===" << std::endl;

    SafePoolThread<50 * 1024 * 1024> global_pool;
    utils::CsvWriter writer("simulation_exotics.csv"); 

    const int num_threads = std::thread::hardware_concurrency();
    const int paths_per_thread = N_TOTAL_PATHS / num_threads;

    std::vector<std::future<double>> futures;

    for (int t = 0; t < num_threads; ++t) {
        
        futures.push_back(std::async(std::launch::async, [&, t]() {
            
            Timepoint t0(std::chrono::nanoseconds(0));
            Engine engine(t0, &global_pool);

            HestonModel::Parameters params { 100.0, 0.04, 0.05, 2.0, 0.04, 0.3, -0.7, 90.0 };
            HestonModel heston(params);

            auto horizon = t0 + std::chrono::hours(365 * 24); 
            auto dt = std::chrono::hours(24); 

            double local_payoff_sum = 0.0;
            
            bool is_logger_thread = (t == 0);
            int max_paths_to_log = 50;

            for (int i = 0; i < paths_per_thread; ++i) {

                bool is_active = true;

                double previous_price = heston.get_price();

                auto barrier_observer = [&](double price){
                    if(price <= heston.get_barrier()){
                        is_active = false;
                        return false;
                    }

                    double B = heston.get_barrier();
                    double dist_prev = previous_price - B;
                    double dist_curr = price - B;

                    double sigma = 0.20;
                    double dt_years = 1.0/365.0;

                    double proba_hit = std::exp(-2.0 * dist_prev * dist_curr / (sigma * sigma * dt_years));

                    static thread_local std::mt19937_64 gen(std::random_device{}());
                    std::uniform_real_distribution<double> dist(0.0, 1.0);
                    if (dist(gen) < proba_hit) {
                        is_active = false;
                        return false;
                    }
                    previous_price = price;
                    return true;
                };
                
                if (is_logger_thread && i < max_paths_to_log) {
                    
                    heston.reset();
                    Timepoint current_time = t0;

                    writer.log_row(0.0, i, 100.0);

                    auto logging_observer = [&](double price){
                        double days = std::chrono::duration<double>(current_time - t0).count() / (24.0 * 3600.0);
                        if (price <= heston.get_barrier()){
                            writer.log_row(days, i, 0.0);
                            is_active = false;
                            return false;
                        }
                        writer.log_row(days, i, price);
                        return true;
                    };

                    while (current_time < horizon) {
                        heston.simulate_path(engine, dt, current_time + dt + std::chrono::microseconds(10), barrier_observer);
                        engine.run(current_time + dt + std::chrono::microseconds(10));
                        
                        current_time += dt;

                        double days = std::chrono::duration<double>(current_time - t0).count() / (24.0 * 3600.0);
                        if (heston.get_price() <= heston.get_barrier()){
                            writer.log_row(days, i, 0.0);
                            break;    
                    }
                    else {
                        writer.log_row(days, i, heston.get_price());
                    }
                
                }

                } else {
                    heston.reset();
                    heston.simulate_path(engine, dt, horizon, barrier_observer);
                    engine.run(horizon);
                }

                if (is_active){
                double final_price = heston.get_price();
                local_payoff_sum += std::max(final_price - 100.0, 0.0);
                }

                else {
                    local_payoff_sum += 0.0;
                }

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