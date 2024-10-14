#ifndef CLUSTER_H
#define CLUSTER_H

#include "../problem/Problem.h"

class Cluster {
    public:
        int* subCluster;
};

int findClosestCustomerInSector(Problem problem, int currentCustomerIndex, int currentSectorIndex, int* freeCustomersIndexes, int* customerSectorMap);
int findClosestCustomer(Problem problem, int currentCustomerIndex, int* freeCustomersIndexes);
void createCluster(Problem problem, int currentCustomerIndex, int subClusterSize, int clusterSectionCount);

#endif