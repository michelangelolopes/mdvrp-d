#ifndef ANT_SYSTEM_H
#define ANT_SYSTEM_H

#include "../problem/ProblemInstance.h"
#include "../solution/Solution.h"

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