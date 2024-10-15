#ifndef STODOLA_INSPIRED_ANT_SYSTEM_H
#define STODOLA_INSPIRED_ANT_SYSTEM_H

#include "../clustering/CustomerCluster.h"
#include "../clustering/Frame.h"
#include "MaxMinAntSystem.h"
#include "SimulatedAnnealing.h"

class StodolaInspiredAntSystem : public MaxMinAntSystem, public SimulatedAnnealing {
    public:
        StodolaInspiredAntSystem(ProblemInstance problemInstance, int sectorsCount, int subClusterSize) : 
        MaxMinAntSystem(problemInstance), 
        frame(problemInstance, sectorsCount)
        {
            create(subClusterSize);
        }

        int localOptimizationFrequency;
        int primaryClustersCount;
        double pheromoneProbabilityCoef;
        double distanceProbabilityCoef;
        CustomerCluster* customerClusters = nullptr;
        Frame frame;

        void create(int subClusterSize);
        void finalize();
        void print();

        void initializeCustomerClusters(int subClusterSize);
        void initializePheromoneMatrix();

        void run();
        Solution buildAntSolution();

        int selectDepot(int vertexIndex, int* visitedCustomerIndexes);
        int selectCluster(int depotIndex, int vertexIndex, int* visitedCustomersIndexes);
        int selectClusterNonPrimary(int vertexIndex, int* visitedCustomersIndexes);
};

int rouletteWheelSelection(int candidatesCount, double* selectionProbability, double selectionProbabilitySum);

#endif