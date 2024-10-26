#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <cmath>

#include "../problem/ProblemInstance.hpp"

#include "Frame.hpp"
#include "SubCluster.hpp"

class Cluster {
    public:
        Cluster() {}
        Cluster(const ProblemInstance& problemInstance, const Frame& frame, int primarySubClustersMaxCount, int subClusterMaxSize, int vertexIndex) {
            create(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, vertexIndex);
        }

        int size;
        int primariesCount;
        SubCluster* subClusters;

        void finalize();
        void print();
        void print(int* visitedCustomersIndexes);

    private:
        void create(const ProblemInstance& problemInstance, const Frame& frame, int primarySubClustersMaxCount, int subClusterMaxSize, int baseIndex);

        void initializeSubClusters(int primarySubClustersMaxCount, int subClusterMaxSize, int neighborcustomersCount);
        void createFirstSubCluster(const ProblemInstance& problemInstance, const Frame& frame, int baseIndex, int* consideredCustomersIndexes);
        void createOthersSubClusters(const ProblemInstance& problemInstance, int baseIndex, int* consideredCustomersIndexes);

        int findClosestCustomer(const ProblemInstance& problemInstance, int baseIndex, int* consideredCustomersIndexes);
        int findClosestCustomerInSector(const ProblemInstance& problemInstance, int currentSectorIndex, int baseIndex, int* consideredCustomersIndexes, int* customerSectorMap);
};

#endif