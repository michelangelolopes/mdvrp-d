#ifndef DEPOT_CUSTOMER_CLUSTER_H
#define DEPOT_CUSTOMER_CLUSTER_H

#include "../problem/ProblemInstance.h"
#include "Cluster.h"
#include "Frame.h"

class DepotToCustomerCluster : public Cluster {
    public:
        DepotToCustomerCluster(ProblemInstance problemInstance, Frame frame, int primarySubClustersMaxCount, int subClusterMaxSize, int depotIndex) {
            create(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, depotIndex);
        }

    private:
        int isCustomerToCustomerCluster() { return 0; }
        int findClosestCustomer(ProblemInstance problemInstance, int depotIndex, int* visitedCustomersIndexes);
        int findClosestCustomerInSector(ProblemInstance problemInstance, int depotIndex, int sectorIndex, int* visitedCustomersIndexes, int* customerSectorMap);
};

#endif