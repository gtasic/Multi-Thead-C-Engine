#include <iostream>
#include <vector>
#include <chrono> 
#include <iomanip>
#include <numeric> // Pour std::accumulate

#include "core/Engine.hpp"
#include "models/GeoBrownian.hpp"

using namespace core;
using namespace models;
using namespace std::chrono_literals;

const int N_PATHS = 1000'000;

int main() {
    std::cout << "=== Monte Carlo Pricing (Call Option) ===" << std::endl;

    std::vector<std::byte> buffer(50 * 1024 * 1024); 
    
    std::pmr::monotonic_buffer_resource pool(buffer.data(), buffer.size(), std::pmr::null_memory_resource());

    Timepoint t0(0ns);
    
    Engine engine(t0, &pool);

    double S0 = 100.0;
    double K = 100.0; 
    GeoBrownian::Parameters params { S0, 0.05, 0.20, 0.0, 1.0 };
    GeoBrownian gbm(params);

    auto horizon = t0 + std::chrono::hours( 30*24); 
    auto dt = std::chrono::hours(1);

    std::vector<double> payoffs;
    payoffs.reserve(N_PATHS);

    std::cout << "Running " << N_PATHS << " simulations..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < N_PATHS; ++i) {
        
        gbm.reset_price(S0); 
        gbm.simulate_path(engine, dt, horizon);
        
        engine.run(horizon);
        double final = gbm.get_price();

        

        double final_price = gbm.get_price();
        double payoff = std::max(final_price - K, 0.0);
        payoffs.push_back(payoff);

        
        engine.reset(t0); 
        pool.release(); 
    }

    auto end = std::chrono::high_resolution_clock::now();


    double total_payoff = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    std::cout<<"le payoff total est "<< total_payoff<<std::endl;
    double average_payoff = total_payoff / N_PATHS;
    
    double r = 0.05;
    double price = average_payoff * std::exp(-r * 1.0);

    std::chrono::duration<double> duration = end - start;

    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Option Price   : " << price << " (Theoretical BS: ~10.45)" << std::endl;
    std::cout << "Total Time     : " << duration.count() << " s" << std::endl;
    std::cout << "Time per Path  : " << (duration.count() / N_PATHS) * 1e6 << " micros" << std::endl;
    std::cout << "Paths/Sec      : " << (long)(N_PATHS / duration.count()) << std::endl;

    return 0;
}