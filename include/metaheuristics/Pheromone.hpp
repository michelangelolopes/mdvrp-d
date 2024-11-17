#ifndef PHEROMONE_HPP
#define PHEROMONE_HPP

#include "../problem/ProblemInstance.hpp"
#include "../solution/Solution.hpp"

class Pheromone {
    public:
        Pheromone(const ProblemInstance& problemInstance, double reinforcementCoef, double evaporationCoefMin, double evaporationCoefMax) :
        problemInstance(&problemInstance),
        reinforcementCoef(reinforcementCoef), 
        evaporationCoefMin(evaporationCoefMin),
        evaporationCoefMax(evaporationCoefMax)
        {
            init();
        }

        double reinforcementValue = 0;
        double evaporationValue = 1;

        double reinforcementCoef;
        double evaporationCoefMin;
        double evaporationCoefMax;
        double*** truckMatrices;
        double*** droneMatrices;

        void finalize();

        void initializeTruckMatrices();
        void initializeDroneMatrices();

        void evaporateTruckMatrices();
        void evaporateDroneMatrices();

        void reinforceTruckMatrices(const Solution& consideredSolution);
        void reinforceDroneMatrices(const Solution& consideredSolution);

        void updateReinforcementValue(const Solution& bestSolution, const Solution& consideredSolution);
        void updateEvaporationValue(double informationEntropy, double informationEntropyMin, double informationEntropyMax);

    private:
        const ProblemInstance* problemInstance;

        void init();
};

#endif