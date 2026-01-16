
#include <memory_resource>
#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>
#include <future>
#include <numeric>

#include "../core/Engine.hpp"
#include "../models/GeoBrownian.hpp"
#include "ThreadRessource.hpp"

using namespace core;
using namespace memory;
using namespace models;


const int N_EV = 1000000;

int main(){

    SafePoolThread<50 * 1024 * 1024> global_pool;

    const int number_thread = std::thread::hardware_concurrency();
    const int number_par_thread = N_EV/number_thread;

    std::cout<< "Launching operation on " << number_thread << "threads" << std::endl;

    std::vector<std::future<double>> futures;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int t =0; t<number_thread; t++){

        futures.push_back(std::async(std::launch::async, [&, t](){
            
            Timepoint t0(std::chrono::nanoseconds(0));
            Engine engine(t0, &global_pool);

            double S0 = 100.0;
            double K = 100.0;
            GeoBrownian::Parameters params(S0, 0.05, 0.20, 0.0,1.0);
            GeoBrownian gbm(params);

            double local_payoff_sum = 0.0;
    
            auto horizon = t0 + std::chrono::hours(30*24);
            auto dt = std::chrono::hours(1);
    

            for(int i =0; i<number_par_thread; i++){

                gbm.reset_price(S0);
                gbm.simulate_path(engine, dt ,horizon);
                engine.run(horizon);
                double final = gbm.get_price();

                local_payoff_sum += std::max(final-K, 0.0);

                engine.reset(t0);
                global_pool.release_current_thread();




            }
            return local_payoff_sum;

        }));
}

    double total_payoff = 0.0;
    for(auto& f : futures){
        total_payoff +=f.get();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    double average = total_payoff/N_EV;

    std::chrono::duration<double> duration = end-start;


    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Option Price   : " << average << " (Theoretical BS: ~10.45)" << std::endl;
    std::cout << "Total Time     : " << duration.count() << " s" << std::endl;
    std::cout << "Time per Path  : " << (duration.count() / N_EV) * 1e6 << " micros" << std::endl;
    std::cout << "Paths/Sec      : " << (long)(N_EV / duration.count()) << std::endl;


    return 0;
}