#ifndef STODOLA_INSPIRED_ANT_SYSTEM_H
#define STODOLA_INSPIRED_ANT_SYSTEM_H

#include "../clustering/CustomerCluster.h"
#include "../clustering/Frame.h"
#include "MaxMinAntSystem.h"
#include "SimulatedAnnealing.h"

class StodolaInspiredAntSystem : public MaxMinAntSystem, public SimulatedAnnealing {
    public:
        StodolaInspiredAntSystem(ProblemInstance problemInstance) : MaxMinAntSystem(problemInstance) {}

        StodolaInspiredAntSystem(ProblemInstance problemInstance, int sectorsCount, int subClusterSize) : MaxMinAntSystem(problemInstance) {
            createFrame(sectorsCount);
            createCustomerClusters(subClusterSize);
            initializePheromoneMatrix();
        }

        int localOptimizationFrequency;
        int primaryClustersCount;
        CustomerCluster* customerClusters = nullptr;
        Frame frame;

        void finalize();
        void createFrame(int sectorsCount);
        void createCustomerClusters(int subClusterSize);
        void initializePheromoneMatrix();

        void printCluster();
};

#endif