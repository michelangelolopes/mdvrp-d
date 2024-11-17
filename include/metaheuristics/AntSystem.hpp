#ifndef ANT_SYSTEM_HPP
#define ANT_SYSTEM_HPP

#include "../problem/ProblemInstance.hpp"
#include "../solution/Solution.hpp"

class AntSystem {
    public:
        AntSystem(const ProblemInstance& problemInstance, int antsCount, double pheromoneUpdateCoef) :
        problemInstance(problemInstance), antsCount(antsCount), pheromoneUpdateCoef(pheromoneUpdateCoef), bestSolution(problemInstance) {}

        int antsCount;
        double pheromoneUpdateCoef;
        double pheromoneEvaporationCoef = 0;
        double*** pheromoneMatrix;

        ProblemInstance problemInstance;
        Solution bestSolution;
};

#endif