#pragma once 
#include "Clock.hpp"
#include <functional>
#include <utility>
#include <compare>


namespace core {
    using EventCallback = std::function<void()>;
    using EventId = uint64_t;

    class Event {
        private : 
        Timepoint m_time;
        EventId m_sequence_id;
        EventCallback m_callback;



        public : 
        Event() = delete;  //An empty event is not considered

        Event(Timepoint time,EventId id , EventCallback&& callback) noexcept: m_time(time), m_sequence_id(id), m_callback(std::move(callback)){

        }
        [[nodiscard]] Timepoint time() const noexcept {
            return m_time;
        }
        [[nodiscard]] EventId sequence_id() const noexcept {
            return m_sequence_id;
        }
        void execute() const {
            if(m_callback){
                m_callback();
            }
        }

    

        auto operator<=>(const Event& other) const noexcept {
            if (auto cmp = m_time <=> other.m_time; cmp != 0) {
                return cmp  ;
            }
            return m_sequence_id <=> other.m_sequence_id;
        }


        bool operator==(const Event&) const = default;
    };
}