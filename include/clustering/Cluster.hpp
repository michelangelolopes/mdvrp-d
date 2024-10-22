#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <cmath>

#include "../problem/ProblemInstance.hpp"

#include "Frame.hpp"
#include "SubCluster.hpp"

class Cluster {
    public:
        Cluster() {}

        int size;
        int primariesCount;
        SubCluster* subClusters;

        void finalize();
        void print();
        void print(int* visitedCustomersIndexes);

    protected:
        void create(ProblemInstance problemInstance, Frame frame, int primarySubClustersMaxCount, int subClusterMaxSize, int baseIndex);

    private:
        void initializeSubClusters(int primarySubClustersMaxCount, int subClusterMaxSize, int neighborcustomersCount);
        void createFirstSubCluster(ProblemInstance problemInstance, Frame frame, int baseIndex, int* consideredCustomersIndexes);
        void createOthersSubClusters(ProblemInstance problemInstance, int baseIndex, int* consideredCustomersIndexes);

        virtual int isCustomerToCustomerCluster() = 0;
        virtual int findClosestCustomer(ProblemInstance problemInstance, int baseIndex, int* consideredCustomersIndexes) = 0;
        virtual int findClosestCustomerInSector(ProblemInstance problemInstance, int currentSectorIndex, int baseIndex, int* consideredCustomersIndexes, int* customerSectorMap) = 0;
};

#endif