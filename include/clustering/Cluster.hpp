#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "../problem/ProblemInstance.hpp"
#include "Frame.hpp"
#include "SubCluster.hpp"

class Cluster {
    public:
        Cluster() {}
        Cluster(const ProblemInstance& problemInstance, const Frame& frame, int primarySubClustersMaxCount, int subClusterMaxSize, int vertexIndex) :
        problemInstance(&problemInstance), frame(&frame) {
            create(primarySubClustersMaxCount, subClusterMaxSize, vertexIndex);
        }

        int size;
        int primariesCount;
        SubCluster* subClusters = nullptr;

        void finalize();
        void print();
        void print(int* visitedCustomersIndexes);

    private:
        const ProblemInstance* problemInstance;
        const Frame* frame;

        void create(int primarySubClustersMaxCount, int subClusterMaxSize, int baseIndex);

        void initializeSubClusters(int primarySubClustersMaxCount, int subClusterMaxSize, int neighborcustomersCount);
        void createFirstSubCluster(int baseIndex, int* consideredCustomersIndexes);
        void createOthersSubClusters(int baseIndex, int* consideredCustomersIndexes);

        int findClosestCustomer(int baseIndex, int* consideredCustomersIndexes);
        int findClosestCustomerInSector(int currentSectorIndex, int baseIndex, int* consideredCustomersIndexes);
};

#endif