#pragma once

#include <cmath>
#include <vector>
#include <random>

#include "../core/Engine.hpp"


namespace models{

    using namespace core;

    class GeoBrownian {
    public : 
    struct Parameters{
        double S0; 
        double drift;
        double vol;
        double mu;
        double sigma;
    };
    
    
    
    GeoBrownian(Parameters& params) : m_params(params), m_current_price(params.S0), m_rng(std::random_device{}()), m_dist(0.0, 1.0){
    }
    
    GeoBrownian(const GeoBrownian&) =delete;
    
    void simulate_path(Engine& engine, std::chrono::nanoseconds dt, Timepoint horizon){
        schedule_next_step(engine,dt,horizon);
    }
    
    [[nodiscard]] double get_price() const {
        return m_current_price;
    }
    void reset_price(double s0){
        m_current_price = s0;
    }
    private : 
    Parameters m_params;
    double m_current_price;
    
    std::mt19937_64 m_rng;
    std::normal_distribution<double> m_dist;  


    void schedule_next_step(Engine& engine, std::chrono::nanoseconds dt, Timepoint horizon){
        Timepoint now = engine.now();

        if(now >= horizon){
            return;
        }
        Timepoint next_time = now +dt;
        if(next_time > horizon){
            next_time = horizon;
        }

        double dt_seconds = std::chrono::duration<double>(dt).count();
        double dt_years = dt_seconds/(3600 * 24 * 365);

        engine.schedule(next_time, [this, &engine, horizon, dt_years, dt](){
        
        double Z = m_dist(m_rng);

        double drift = (m_params.mu - 0.5 * m_params.sigma * m_params.sigma) * dt_years;
        double diffusion = m_params.sigma * std::sqrt(dt_years) * Z;
                
        m_current_price = m_current_price * std::exp(drift + diffusion);

        schedule_next_step(engine, dt ,horizon);
        });
    }

    
    
};
}