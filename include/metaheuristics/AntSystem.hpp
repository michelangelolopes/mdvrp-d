#ifndef ANT_SYSTEM_HPP
#define ANT_SYSTEM_HPP

#include "../problem/ProblemInstance.hpp"
#include "../solution/Solution.hpp"

class AntSystem {
    public:
        AntSystem(const ProblemInstance& problemInstance, int antsCount, double pheromoneReinforcementCoef) :
        problemInstance(problemInstance), antsCount(antsCount), pheromoneReinforcementCoef(pheromoneReinforcementCoef), bestSolution(problemInstance) {}

        int antsCount;
        double pheromoneReinforcementCoef;
        double pheromoneEvaporationCoef = 0;
        double*** pheromoneMatrix;

        ProblemInstance problemInstance;
        Solution bestSolution;
};

#endif