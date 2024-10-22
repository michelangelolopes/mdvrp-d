#ifndef ANT_SYSTEM_HPP
#define ANT_SYSTEM_HPP

#include "../problem/ProblemInstance.hpp"
#include "../solution/Solution.hpp"

class AntSystem {
    public:
        AntSystem(ProblemInstance problemInstance, int antsCount, double pheromoneUpdateCoef) :
        problemInstance(problemInstance) 
        {
            this->antsCount = antsCount;
            this->pheromoneUpdateCoef = pheromoneUpdateCoef;
        } 

        int antsCount;
        double pheromoneUpdateCoef;
        double pheromoneEvaporationCoef = 0;
        double*** pheromoneMatrix;

        ProblemInstance problemInstance;
        Solution* bestSolution = nullptr;

        void finalize() {
            problemInstance.finalize();
            
            if(bestSolution != nullptr) {
                bestSolution->finalize();
                delete bestSolution;
            }
        }
};

#endif