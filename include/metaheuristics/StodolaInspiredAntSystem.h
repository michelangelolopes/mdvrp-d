#ifndef STODOLA_INSPIRED_ANT_SYSTEM_H
#define STODOLA_INSPIRED_ANT_SYSTEM_H

#include "../clustering/CustomerCluster.h"
#include "../clustering/Frame.h"
#include "MaxMinAntSystem.h"
#include "SimulatedAnnealing.h"

class StodolaInspiredAntSystem : public MaxMinAntSystem, public SimulatedAnnealing {
    public:
        StodolaInspiredAntSystem(Problem problem) {
            this->problem = problem;
        }

        StodolaInspiredAntSystem(Problem problem, int sectorsCount, int subClusterSize) {
            this->problem = problem;
            createFrame(sectorsCount);
            createCustomerClusters(subClusterSize);
        }

        int localOptimizationFrequency;
        int primaryClustersCount;
        CustomerCluster* customerClusters = nullptr;
        Problem problem;
        Frame frame;

        void finalize();
        void createFrame(int sectorsCount);
        void createCustomerClusters(int subClusterSize);

        void printCluster();
};

#endif