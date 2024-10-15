#ifndef ANT_SYSTEM_H
#define ANT_SYSTEM_H

#include "../solution/Solution.h"

class AntSystem {
    public:
        AntSystem(ProblemInstance problemInstance) {
            this->problemInstance = problemInstance;
            this->routeMaxLength = (problemInstance.customerCount * 2) + 1; //route worst case: (depot, customer), (depot, customer), ..., depot
        }

        int antsCount;
        int routeMaxLength;
        double pheromoneUpdateCoef;
        double pheromoneProbabilityCoef;
        double distanceProbabilityCoef;
        double*** pheromoneMatrix;

        ProblemInstance problemInstance;
        Solution* antsSolution;
};

#endif