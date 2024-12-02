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

        double*** droneMatrices;

        void finalize() override;
        void evaporateMatrices(const InformationEntropy& informationEntropy) override;
        void reinforceMatrices(const Solution& bestSolution, const Solution& consideredSolution) override;

    protected:
        void init() override;
};

#endif