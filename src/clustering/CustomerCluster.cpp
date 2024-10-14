#include "../../include/clustering/CustomerCluster.h"

#include <cmath>
#include <iostream>

#include "../../include/clustering/Frame.h"
#include "../../include/utils/ArrayUtils.h"
#include "../../include/utils/MathUtils.h"

int findClosestNeighborInSector(Problem problem, int currentCustomerIndex, int currentSectorIndex, int* freeCustomersIndexes, int* customerSectorMap) {

    int closestNeighborIndex = -1;
    double closestCustomerDistance = -1;
    for(int neighborIndex = 0; neighborIndex < problem.customerCount; neighborIndex++) {
        if(freeCustomersIndexes[neighborIndex] != 1 && customerSectorMap[neighborIndex] == currentSectorIndex) {
            double neighborCustomerDistance = problem.customerDistanceMatrix[currentCustomerIndex][neighborIndex];

            // std::cout << neighborCustomerDistance << '\n';

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestNeighborIndex = neighborIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestNeighborIndex;
}

int findClosestNeighbor(Problem problem, int currentCustomerIndex, int* freeCustomersIndexes) {

    int closestNeighborIndex = -1;
    double closestCustomerDistance = -1;
    for(int neighborIndex = 0; neighborIndex < problem.customerCount; neighborIndex++) {
        if(freeCustomersIndexes[neighborIndex] != 1) {
            double neighborCustomerDistance = problem.customerDistanceMatrix[currentCustomerIndex][neighborIndex];

            // std::cout << neighborCustomerDistance << '\n';

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestNeighborIndex = neighborIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestNeighborIndex;
}

void CustomerCluster::create(Problem problem, Frame frame) {
    this->neighborCount = problem.customerCount - 1;
    this->clusterSize = std::ceil((float) this->neighborCount / (float) this->subClusterSize);
    this->clusters = (int**) initialize(this->clusterSize, sizeof(int*));
    
    std::cout << "this->neighborCount " << this->neighborCount << "\n";
    std::cout << "this->clusterSize " << this->clusterSize << "\n";

    int* freeCustomersIndexes = (int*) initialize(problem.customerCount, sizeof(int));
    int freeCustomersCount = this->neighborCount;


    freeCustomersIndexes[this->customerIndex] = 1; //already visited

    for(int clusterIndex = 0; clusterIndex < this->clusterSize; clusterIndex++) {
        this->clusters[clusterIndex] = (int*) initialize(this->subClusterSize, sizeof(int));
    }

    // int clusterCount = std::ceil((float) freeCustomersCount / (float) this->subClusterSize);
    // std::cout << "clusterCount " << this->clusterSize << "\n";
    // std::cout << "this->clusters\n";
    // std::cout << subClusterCount << '\n';
    // int** cluster = (int**) initialize(clusterSize, sizeof(int*));

    // int* subCluster = (int*) initialize(this->subClusterSize, sizeof(int));
    // std::cout << "subCluster -- clusters " << clusterIndex << '\n';
    int closestNeighborIndex;

    for(int sectionIndex = 0; sectionIndex < frame.sectorsCount; sectionIndex++) {
        closestNeighborIndex = findClosestNeighborInSector(problem, this->customerIndex, sectionIndex, freeCustomersIndexes, frame.customerSectorMap);
        
        if(closestNeighborIndex != -1) {
            std::cout << "closestNeighborIndex " << closestNeighborIndex << '\n';
            this->clusters[0][sectionIndex] = closestNeighborIndex;
            freeCustomersIndexes[closestNeighborIndex] = 1; //visited
            freeCustomersCount--;
        }
    }


    std::cout << "this->neighborCount - freeCustomersCount " << this->neighborCount - freeCustomersCount << "\n";
    std::cout << "frame.sectorsCount " << frame.sectorsCount << "\n";
    for(int subClusterIndex = this->neighborCount - freeCustomersCount; subClusterIndex < this->subClusterSize; subClusterIndex++) {
        closestNeighborIndex = findClosestNeighbor(problem, this->customerIndex, freeCustomersIndexes);
        this->clusters[0][subClusterIndex] = closestNeighborIndex;
        freeCustomersIndexes[closestNeighborIndex] = 1; //visited
        freeCustomersCount--;
    }

    // for(int subClusterIndex = 0; subClusterIndex < this->subClusterSize; subClusterIndex++) {
    //     int neighborIndex = subCluster[subClusterIndex];
    //     int neighborDistance = problem.customerDistanceMatrix[this->customerIndex][neighborIndex];
    //     int neighborSector = frame.customerSectorMap[neighborIndex];

    //     std::cout << "--------\n";
    //     std::cout << "SubCluster " << subClusterIndex << "\n";
    //     std::cout << "Neighbor " << neighborIndex << "\n";
    //     std::cout << "Distance " << neighborDistance << "\n";
    //     std::cout << "Sector " << neighborSector << "\n";
    //     std::cout << "--------\n";
    // }

    // this->cluster[clusterIndex] = subCluster;
    // clusterIndex++;
    

    // findClosestNeighbor(problem, currentCustomerIndex, freeCustomersIndexes, customerSectorMap);

    int clusterIndex = 1;
    int subClusterIndex = 0;
    std::cout << "subClusterSize " << subClusterSize << '\n';
    std::cout << "subClusterIndex " << subClusterIndex << '\n';
    while(freeCustomersCount > 0) {
        std::cout << "clusterIndex " << clusterIndex << '\n';
        std::cout << "subClusterIndex " << subClusterIndex << '\n';
        std::cout << "freeCustomersCount " << freeCustomersCount << '\n';
        if(subClusterIndex == subClusterSize) {

            // for(int neighborIndex = 0; neighborIndex < problem.customerCount; neighborIndex++) {
            //     if(freeCustomersIndexes[neighborIndex] == 0) {
            //         std::cout << neighborIndex << ' ';
            //     }
            // }
            // std::cout << '\n';

            subClusterIndex = 0;
            // std::cout << "subCluster mem " << subCluster << '\n';
            // this->clusters[clusterIndex] = subCluster;
            // std::cout << "clusters mem " << this->clusters[clusterIndex] << '\n';
            clusterIndex++;
            // subCluster = nullptr;
            // subCluster = (int*) initialize(subClusterSize, sizeof(int));
            std::cout << "subCluster -- clusters " << clusterIndex << '\n';
        }

        closestNeighborIndex = findClosestNeighbor(problem, this->customerIndex, freeCustomersIndexes);
        this->clusters[clusterIndex][subClusterIndex] = closestNeighborIndex;
        freeCustomersIndexes[closestNeighborIndex] = 1; //visited
        
        subClusterIndex++;
        freeCustomersCount--;
    }

    free(freeCustomersIndexes);

    // std::cout << closestCustomerIndex << '\n';


    return; 
}

void CustomerCluster::finalize() {
    if(this->clusters != nullptr) {
        for(int clusterIndex = 0; clusterIndex < this->clusterSize; clusterIndex++) {
            free(this->clusters[clusterIndex]);
        }
        free(this->clusters);
    }
}