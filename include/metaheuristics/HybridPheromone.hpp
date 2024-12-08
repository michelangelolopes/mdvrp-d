#ifndef HYBRID_PHEROMONE_HPP
#define HYBRID_PHEROMONE_HPP

#include "Pheromone.hpp"

class HybridPheromone : public Pheromone {
    public:
        HybridPheromone(const ProblemInstance& problemInstance, double reinforcementCoef, double evaporationCoefMin, double evaporationCoefMax) :
        Pheromone(problemInstance, reinforcementCoef, evaporationCoefMin, evaporationCoefMax)
        {
            init();
        }

        double*** droneMatrices = nullptr;

        void finalize();
        void evaporateMatrices(const InformationEntropy& informationEntropy);
        void reinforceMatrices(const Solution& bestSolution, const Solution& consideredSolution);

    protected:
        void init();
};

#endif