#ifndef CLUSTER_H
#define CLUSTER_H

#include "../problem/ProblemInstance.h"
#include "Frame.h"

class Cluster {
    public:
        Cluster(int vertexIndex, int subClusterSize) {
            this->vertexIndex = vertexIndex;
            this->subClusterSize = subClusterSize;
        }

        Cluster(ProblemInstance problemInstance, Frame frame, int vertexIndex, int subClusterSize) : 
        Cluster(vertexIndex, subClusterSize)
        {
            create(problemInstance, frame);
        }

        int clusterSize;
        int isCustomerVertex;
        int neighborCustomerCount;
        int subClusterSize;
        int vertexIndex;
        int** clusters;

        void create(ProblemInstance problemInstance, Frame frame);
        void finalize();
        void print();
};

int findClosestCustomer(ProblemInstance problemInstance, int vertexIndex, int* visitedCustomersIndexes);
int findClosestCustomerInSector(ProblemInstance problemInstance, int vertexIndex, int currentSectorIndex, int* visitedCustomersIndexes, int* customerSectorMap);

#endif