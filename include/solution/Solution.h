#ifndef SOLUTION_H
#define SOLUTION_H

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.h"

#include "Route.h"

class Solution {
    public:
        Solution(int depotsCount, int subRouteMaxLength) : depotsCount(depotsCount) {
            initialize(subRouteMaxLength);
        }

        int depotsCount;
        double fitness = -1;
        double distanceTraveled = -1;
        double timeSpent = -1;
        Route* routes;

        void finalize();
        void updateFitness(ProblemInstance problemInstance);

        void print() const;

    private:
        void initialize(int subRouteMaxLength);
};

#endif