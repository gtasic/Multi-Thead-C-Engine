# pragma once 
#include <chrono>
#include "../utils/assert.hpp"
 
namespace core {

    struct SimuClock{
        using duration = std::chrono::nanoseconds; 
        using rep = duration::rep;
        using period = duration::period;
        static constexpr bool is_steady = true;
    };

    using Timepoint = std::chrono::time_point<SimuClock>;

    class Clock {
        private : 
        Timepoint m_now;
        const Timepoint m_start_point;

        public : 
        constexpr Clock() noexcept : m_now(std::chrono::nanoseconds(0)), m_start_point(std::chrono::nanoseconds(0)){
        }

        explicit constexpr Clock(Timepoint start_time) noexcept : m_now(start_time), m_start_point(start_time){
        }

        ~Clock()= default;
        Clock(const Clock&) = delete;
        Clock& operator=(const Clock&) = delete;
        
        [[nodiscard]] constexpr Timepoint now() const noexcept{
            return m_now;
        }

        void advanced_to(Timepoint target_time) noexcept{
            SIM_ASSERT(target_time >= m_now, "Clock consistency error: Time cannot move backward");

            m_now = target_time;
        }

        void reset() {
            m_now = m_start_point;
        }
    };

}