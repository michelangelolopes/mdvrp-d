#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <cstdlib>
#include <iostream>

#include "../enum/MinimizationTypeEnum.hpp"
#include "../problem/ProblemInstance.hpp"

#include "Route.hpp"
#include "DroneRoute.hpp"

class Solution {
    public:
        Solution() {}
        Solution(int depotsCount, const MinimizationType& minimizationType) : depotsCount(depotsCount), minimizationType(minimizationType) {}
        Solution(int depotsCount, const MinimizationType& minimizationType, int subRouteMaxLength) : 
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
        DroneRoute* droneRoutes;

        void finalize();
        void reset();
        void copy(Solution solutionToCopy);
        void updateFitness(const ProblemInstance& problemInstance);
        void updateFitnessWithDrone(const ProblemInstance& problemInstance);

        double calculateRouteDuration(const ProblemInstance& problemInstance, int depotIndex);

        bool checkConstraints(const ProblemInstance& problemInstance) const;
        void print() const;
        void printWithDrone() const;
        void printWithDrone(int depotIndex) const;
    private:
        void init(int subRouteMaxLength);
        void initializeValues();
        void initializeRoutes(int subRouteMaxLength);
        void initializeDroneRoutes(int maxSize);
};

#endif