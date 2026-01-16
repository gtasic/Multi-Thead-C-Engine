#pragma once 

#include <memory>
#include <chrono> 
#include <memory_resource>
#include <functional>
#include <atomic>

#include "../errors/common.hpp"
#include "Scheduler.hpp"
#include "Clock.hpp"

namespace core {

    class Engine {

        public : 
        Engine(Timepoint start_time,std::pmr::memory_resource* mr = std::pmr::get_default_resource()) : clock_(start_time) , scheduler_( clock_, mr){

        }
        Engine(const Engine&) = delete; // We do not create an engine from an engine
        Engine& operator=(const Engine&) = delete; //We do not associate an Engine with another one 

        void schedule(Timepoint time, EventCallback callback){
            scheduler_.schedule(time, std::move(callback));
        }

        [[nodiscard]] Timepoint now() const noexcept{
            return clock_.now();
        }

        void stop(){
            keep_running = false;
        }


        void reset(Timepoint start_time){
            clock_.reset();
            scheduler_.clear();
            keep_running = false;
        }

        void run(Timepoint stopping_time = Timepoint::max()) {
            keep_running = true;

            std::chrono::milliseconds timeout_duration = std::chrono::milliseconds(5000);
            auto start = std::chrono::steady_clock::now();

            while(keep_running && !scheduler_.empty()){
                if(clock_.now() >= stopping_time){
                    break;
                }

                auto wall_now = std::chrono::steady_clock::now();
                if(wall_now - start > timeout_duration){
                    throw errors::InfiniteLoopError();
                }
                scheduler_.step();
            }
            keep_running = false;
            }
        
        Scheduler& get_shed(){
            return scheduler_;
        }
        
        

        private : 
        std::atomic<bool> keep_running{false};
        Clock clock_;
        Scheduler scheduler_;

    };
}
