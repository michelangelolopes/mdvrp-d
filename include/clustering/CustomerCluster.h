#ifndef CUSTOMER_CLUSTER_H
#define CUSTOMER_CLUSTER_H

#include "../problem/ProblemInstance.h"
#include "Frame.h"

class CustomerCluster {
    public:
        CustomerCluster(int vertexIndex, int subClusterSize) {
            this->vertexIndex = vertexIndex;
            this->subClusterSize = subClusterSize;
        }

        CustomerCluster(ProblemInstance problemInstance, Frame frame, int vertexIndex, int subClusterSize) : 
        CustomerCluster(vertexIndex, subClusterSize)
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