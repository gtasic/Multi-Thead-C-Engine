#include <iostream>
#include <array>
#include <vector>

#include "../core/Engine.hpp"
#include "GeoBrownian.hpp"

using namespace core;
using namespace models;

int main(){

    Timepoint t0(std::chrono::nanoseconds(0));

    std::vector<std::byte> buffer(1024*1024*200);
    std::pmr::monotonic_buffer_resource pool(buffer.data(), buffer.size(), std::pmr::null_memory_resource());

    Engine engine(t0, &pool);

    GeoBrownian::Parameters params{100.0, 0.05, 0.20, 0.0, 1.0};
    GeoBrownian gbm(params);


    auto horizon = t0 + std::chrono::hours(24*365);
    auto dt = std::chrono::hours(24);


    gbm.simulate_path(engine, dt,horizon);

    auto start_time = std::chrono::high_resolution_clock::now();
    engine.run();
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end_time - start_time;
    std::cout<<"Final price "<< gbm.get_price()<<std::endl;
    std::cout<<"The duration was "<< duration<<std::endl;
    

    return 0;

}
