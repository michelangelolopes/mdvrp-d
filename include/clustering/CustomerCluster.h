#ifndef CUSTOMER_CLUSTER_H
#define CUSTOMER_CLUSTER_H

#include "Frame.h"
#include "../problem/ProblemInstance.h"

class CustomerCluster {
    public:
        CustomerCluster(int vertexIndex, int subClusterSize) {
            this->vertexIndex = vertexIndex;
            this->subClusterSize = subClusterSize;
        }
        int neighborCustomerCount;
        int clusterSize;
        int vertexIndex;
        int subClusterSize;
        int** clusters;

        void create(ProblemInstance problemInstance, Frame frame);
        void print();
        void finalize();
};

int findClosestCustomerInSector(ProblemInstance problemInstance, int currentVertexIndex, int currentSectorIndex, int* visitedCustomersIndexes, int* customerSectorMap);
int findClosestCustomer(ProblemInstance problemInstance, int currentVertexIndex, int* visitedCustomersIndexes);
void createCluster(ProblemInstance problemInstance, int currentVertexIndex, int subClusterSize, int clusterSectionCount);

#endif