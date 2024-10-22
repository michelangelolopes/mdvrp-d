#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <cstdlib>
#include <iostream>

#include "../enum/MinimizationTypeEnum.hpp"
#include "../problem/ProblemInstance.hpp"

#include "Route.hpp"

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