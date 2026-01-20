#include <cmath>
#include <vector>
#include <random>


#include "../core/Engine.hpp"

namespace models{
    
using namespace core;

    class HestonModel{
        public :
        struct Parameters{
            double S0;
            double v0;
            double mu;
            double kappa;
            double theta;
            double xi;
            double rho;
        };


        HestonModel(Parameters& params) : m_params(params), m_current_price(params.S0),m_current_variance(params.v0) ,m_rng(std::random_device{}()), m_dist(0.0,1.0){

        }
        HestonModel(const HestonModel&) = delete;

      

        

        void simulate_path(Engine& engine, std::chrono::nanoseconds dt, Timepoint horizon){
            schedule_next_step(engine,dt,horizon);
        }

        [[nodiscard]] double get_price() const {
            return m_current_price;
        }
        void reset(){
            m_current_price = m_params.S0;
            m_current_variance = m_params.v0;
        }

        private : 
        Parameters m_params;
        double m_current_price;
        double m_current_variance;
        std::mt19937_64 m_rng;
        std::normal_distribution<double> m_dist;


        void schedule_next_step(Engine& engine, std::chrono::nanoseconds dt, Timepoint horizon){
            Timepoint now = engine.now();

            if (now >= horizon){
                return;
            }
            Timepoint next_time = now + dt;

            if (next_time > horizon){
                next_time = horizon;
            }

            double dt_years = std::chrono::duration<double>(dt).count()/(3600 * 24 * 365);
            double square_dt = std::sqrt(dt_years);

            engine.schedule(next_time, [this, &engine, horizon, dt_years, square_dt, dt](){

                double Z1 = m_dist(m_rng);
                double Z2 = m_dist(m_rng);

                double W1 = Z1;
                double W2 = m_params.rho * Z1 + std::sqrt(1 - m_params.rho * m_params.rho) * Z2;


                double v_positive = std::max(m_current_variance, 0.0);
                double sqrt_v = std::sqrt(v_positive);

                double dv = m_params.kappa * (m_params.theta - v_positive) * dt_years + m_params.xi * sqrt_v * square_dt * W2;

                m_current_variance += dv;

                double drift = m_params.mu * dt_years;
                double diffusion = sqrt_v * square_dt * W1;

                m_current_price *= std::exp((m_params.mu - 0.5 * v_positive) * dt_years + diffusion);
                schedule_next_step(engine, dt, horizon);


            });

            
        }

    };
    }
