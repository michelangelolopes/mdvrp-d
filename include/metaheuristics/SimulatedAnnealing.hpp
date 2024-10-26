#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP

class SimulatedAnnealing {
    public:
        SimulatedAnnealing(double temperatureUpdateCoef, double temperatureCoolingCoef) :
        temperatureUpdateCoef(temperatureUpdateCoef), temperatureCoolingCoef(temperatureCoolingCoef) {}

        double temperatureUpdateCoef;
        double temperatureCoolingCoef;
};

#endif