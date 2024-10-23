#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include <cmath>

#include "../problem/ProblemInstance.hpp"

#include "Frame.hpp"
#include "SubCluster.hpp"

class Cluster {
    public:
        Cluster() {}
        Cluster(ProblemInstance problemInstance, Frame frame, int primarySubClustersMaxCount, int subClusterMaxSize, int vertexIndex) {
            create(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, vertexIndex);
        }

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

        int findClosestCustomer(ProblemInstance problemInstance, int baseIndex, int* consideredCustomersIndexes);
        int findClosestCustomerInSector(ProblemInstance problemInstance, int currentSectorIndex, int baseIndex, int* consideredCustomersIndexes, int* customerSectorMap);
};

#endif