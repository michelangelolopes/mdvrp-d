#ifndef SOLUTION_H
#define SOLUTION_H

#include <cstdlib>
#include <iostream>

#include "../enum/MinimizationTypeEnum.h"
#include "../problem/ProblemInstance.h"

#include "Route.h"

class Solution {
    public:
        Solution(int depotsCount, MinimizationType minimizationType, int subRouteMaxLength) : 
        depotsCount(depotsCount),
        minimizationType(minimizationType) 
        {
            initialize(subRouteMaxLength);
        }

        double fitness = -1;
        double totalDistanceTraveled = -1;
        double totalTimeSpent = -1;
        double maxDistanceTraveled = -1;
        double maxTimeSpent = -1;
        
        int depotsCount;
        MinimizationType minimizationType;
        Route* routes;

        void finalize();
        void updateFitness(ProblemInstance problemInstance);

        void print() const;

    private:
        void initialize(int subRouteMaxLength);
};

#endif