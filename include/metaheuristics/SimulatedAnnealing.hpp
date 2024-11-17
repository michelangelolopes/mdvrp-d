#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP

class SimulatedAnnealing {
    public:
        SimulatedAnnealing(double temperature, double temperatureCoolingCoef) :
        temperature(temperature), temperatureCoolingCoef(temperatureCoolingCoef) {}

        double temperature;
        double temperatureCoolingCoef;

        inline void coolDownTemperature() {
            temperature *= temperatureCoolingCoef;
        }
};

#endif