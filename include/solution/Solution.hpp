#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "../problem/ProblemInstance.hpp"
#include "Route.hpp"

class Solution {
    public:
        Solution(const ProblemInstance& problemInstance) : problemInstance(&problemInstance) {
            init();
        }

        double fitness;
        double totalDistanceTraveled;
        double totalTimeSpent;
        double maxDistanceTraveled;
        double maxTimeSpent;

        Route* routes = nullptr;

        void finalize();
        void reset();
        void copy(Solution solutionToCopy);
        void updateFitness();
        void updateFitnessWithDrone();

        bool checkConstraints() const;
        void print() const;
        void printWithDrone() const;
        void printWithDrone(int depotIndex) const;
    private:
        const ProblemInstance* problemInstance;

        void init();
        void initializeValues();
        void initializeRoutes();
};

#endif