#ifndef CUSTOMER_CUSTOMER_CLUSTER_HPP
#define CUSTOMER_CUSTOMER_CLUSTER_HPP

#include "../problem/ProblemInstance.hpp"
#include "Cluster.hpp"
#include "Frame.hpp"

class CustomerToCustomerCluster : public Cluster {
    public:
        CustomerToCustomerCluster(ProblemInstance problemInstance, Frame frame, int primarySubClustersMaxCount, int subClusterMaxSize, int customerIndex) {
            create(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, customerIndex);
        }

    private:
        int isCustomerToCustomerCluster() { return 1; }
        int findClosestCustomer(ProblemInstance problemInstance, int customerIndex, int* visitedCustomersIndexes);
        int findClosestCustomerInSector(ProblemInstance problemInstance, int customerIndex, int sectorIndex, int* visitedCustomersIndexes, int* customerSectorMap);        
};

#endif