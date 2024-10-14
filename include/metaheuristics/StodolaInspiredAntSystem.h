#ifndef STODOLA_INSPIRED_ANT_SYSTEM_H
#define STODOLA_INSPIRED_ANT_SYSTEM_H

#include "../clustering/Cluster.h"
#include "../clustering/Frame.h"
#include "MaxMinAntSystem.h"
#include "SimulatedAnnealing.h"

class StodolaInspiredAntSystem : public MaxMinAntSystem, public SimulatedAnnealing {
    public:
        int localOptimizationFrequency;
        int primaryClustersCount;
        int subClusterSize;
        // int sectorsCount;
        Cluster* clusters;
        Frame frame;
};

#endif