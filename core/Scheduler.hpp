#pragma once 

#include <memory_resource>
#include <queue>
#include <vector>
#include <functional> 

#include "Clock.hpp"
#include "Event.hpp"
#include "../errors/common.hpp"
#include "../utils/assert.hpp"


namespace core {
    using EventList = std::priority_queue<Event, std::pmr::vector<Event>, std::greater<Event>>;

    class Scheduler{
        private : 
        Clock& m_clock;
        std::pmr::memory_resource* m_mr;
        EventList eventlist;
        uint64_t m_next_sequence_id =0;

        public : 
        constexpr Scheduler(const Scheduler&) = delete; 
        explicit Scheduler(Clock& clock, std::pmr::memory_resource* mr = std::pmr::get_default_resource()) : m_clock(clock), m_mr(mr), eventlist(std::greater<Event>(), std::pmr::vector<Event>(mr)){
        };
        Scheduler& operator=(const Scheduler&) = delete; // Do not copi the scheduler it is unique. 
        
        EventList get_list(){
            return eventlist;
        }
        void schedule(Timepoint time, EventCallback callback) {
            if(time < m_clock.now()){
                throw errors::PastEventError(time.time_since_epoch().count(),m_clock.now().time_since_epoch().count());
            }
            eventlist.emplace(time, m_next_sequence_id++, std::move(callback));
        };

        void step(){
            if(eventlist.empty()) return;
            m_clock.advanced_to(eventlist.top().time());
            const Event& current_event = eventlist.top();
            current_event.execute();
            eventlist.pop();     
        }
        [[nodiscard]] bool empty() const noexcept {
            return eventlist.empty();
        }
        [[nodiscard]] size_t size() const noexcept{
            return eventlist.size();
        }
        


        void clear(){
            EventList emptylist{std::greater<Event>(), std::pmr::vector<Event>(m_mr)};
            eventlist.swap(emptylist);
            m_next_sequence_id = 0;
        }
    };
};