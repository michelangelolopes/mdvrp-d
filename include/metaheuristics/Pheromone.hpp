#ifndef PHEROMONE_HPP
#define PHEROMONE_HPP

#include "../problem/ProblemInstance.hpp"
#include "../solution/Solution.hpp"
#include "InformationEntropy.hpp"

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

        virtual void finalize();
        virtual void evaporateMatrices(const InformationEntropy& informationEntropy);
        virtual void reinforceMatrices(const Solution& bestSolution, const Solution& consideredSolution);

    protected:
        const ProblemInstance* problemInstance;

        virtual void init();
        void updateReinforcementValue(const Solution& bestSolution, const Solution& consideredSolution);
        void updateEvaporationValue(const InformationEntropy& informationEntropy);
};

#endif