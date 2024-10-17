#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

class SimulatedAnnealing {
    public:
        SimulatedAnnealing(double temperatureUpdateCoef, double temperatureCoolingCoef) {
            this->temperatureUpdateCoef = temperatureUpdateCoef;
            this->temperatureCoolingCoef = temperatureCoolingCoef;
        }

        double temperatureUpdateCoef;
        double temperatureCoolingCoef;
};

#endif