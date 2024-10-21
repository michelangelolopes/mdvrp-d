#ifndef SOLUTION_H
#define SOLUTION_H

#include <cstdlib>
#include <iostream>

#include "../enum/MinimizationTypeEnum.h"
#include "../problem/ProblemInstance.h"

#include "Route.h"

class Solution {
    public:
        Solution(int depotsCount, MinimizationType minimizationType) : depotsCount(depotsCount), minimizationType(minimizationType) {}

        Solution(int depotsCount, MinimizationType minimizationType, int subRouteMaxLength) : 
        Solution(depotsCount, minimizationType)
        {
            init(subRouteMaxLength);
        }

        double fitness;
        double totalDistanceTraveled;
        double totalTimeSpent;
        double maxDistanceTraveled;
        double maxTimeSpent;
        
        int depotsCount;
        MinimizationType minimizationType;
        Route* routes;

        void finalize();
        void reset();
        void copy(Solution solutionToCopy);
        void updateFitness(ProblemInstance problemInstance);

        void print() const;
    private:
        void init(int subRouteMaxLength);
        void initializeValues();
        void initializeRoutes(int subRouteMaxLength);
};

#endif