#pragma once 
#include <stdexcept>
#include <string>
#include <format>
#include <cstdint>


namespace errors{

    class SimulationErrors : public std::runtime_error {
        public : 
        using std::runtime_error::runtime_error;
    };

    class PastEventError : public SimulationErrors{

        public : 
        PastEventError(uint64_t event_time, uint64_t current_time) : 
        SimulationErrors(std::format("Consistency Error : Attempted to schedule event at {} while clock is at {}", event_time, current_time))
        {}
    };
    class EmptyQueueError : public SimulationErrors {
        public : 
        EmptyQueueError() : SimulationErrors("ScheduleError : Attempted to pop from an empty queue"){
            
        }
    };
    class InfiniteLoopError : public SimulationErrors{
        public : 
        InfiniteLoopError() : SimulationErrors("Infinite loop in the scheduler"){
            
        }
    };
}