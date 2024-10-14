#ifndef CUSTOMER_CLUSTER_H
#define CUSTOMER_CLUSTER_H

#include "Frame.h"
#include "../problem/Problem.h"

class CustomerCluster {
    public:
        CustomerCluster(int customerIndex, int subClusterSize) {
            this->customerIndex = customerIndex;
            this->subClusterSize = subClusterSize;
        }
        int neighborCount;
        int clusterSize;
        int customerIndex;
        int subClusterSize;
        int** clusters;

        void create(Problem problem, Frame frame);
        void print();
        void finalize();
};

int findClosestCustomerInSector(Problem problem, int currentCustomerIndex, int currentSectorIndex, int* visitedCustomersIndexes, int* customerSectorMap);
int findClosestCustomer(Problem problem, int currentCustomerIndex, int* visitedCustomersIndexes);
void createCluster(Problem problem, int currentCustomerIndex, int subClusterSize, int clusterSectionCount);

#endif