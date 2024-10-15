#include "../../include/clustering/CustomerCluster.h"

#include <cmath>
#include <iostream>

#include "../../include/clustering/Frame.h"
#include "../../include/utils/ArrayUtils.h"
#include "../../include/utils/MathUtils.h"

int findClosestNeighborInSector(ProblemInstance problemInstance, int vertexIndex, int currentSectorIndex, int* visitedCustomersIndexes, int* customerSectorMap) {

    int closestCustomerIndex = -1;
    double closestCustomerDistance = -1;
    for(int customerIndex = 0; customerIndex < problemInstance.customerCount; customerIndex++) {
        if(visitedCustomersIndexes[customerIndex] != 1 && customerSectorMap[customerIndex] == currentSectorIndex) {
            double neighborCustomerDistance = problemInstance.distanceMatrix[vertexIndex][customerIndex];

            // std::cout << neighborCustomerDistance << '\n';

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestCustomerIndex = customerIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestCustomerIndex;
}

int findClosestNeighbor(ProblemInstance problemInstance, int vertexIndex, int* visitedCustomersIndexes) {

    int closestCustomerIndex = -1;
    double closestCustomerDistance = -1;
    for(int customerIndex = 0; customerIndex < problemInstance.customerCount; customerIndex++) {
        if(visitedCustomersIndexes[customerIndex] != 1) {
            double neighborCustomerDistance = problemInstance.distanceMatrix[vertexIndex][customerIndex];

            // std::cout << neighborCustomerDistance << '\n';

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestCustomerIndex = customerIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestCustomerIndex;
}

void CustomerCluster::create(ProblemInstance problemInstance, Frame frame) {
    
    int* visitedCustomersIndexes = (int*) initialize(problemInstance.customerCount, sizeof(int));

    this->neighborCustomerCount = problemInstance.customerCount;
    if(this->vertexIndex < problemInstance.customerCount) {
        this->neighborCustomerCount--; //a customer does not considers itself
        visitedCustomersIndexes[this->vertexIndex] = 1; //already visited
    }
    this->clusterSize = std::ceil((float) this->neighborCustomerCount / (float) this->subClusterSize);
    this->clusters = (int**) initialize(this->clusterSize, this->subClusterSize, sizeof(int*), sizeof(int));
    
    std::cout << "this->neighborCustomerCount " << this->neighborCustomerCount << "\n";
    std::cout << "this->clusterSize " << this->clusterSize << "\n";

    int unvisitedCustomersCount = this->neighborCustomerCount;



    // int clusterCount = std::ceil((float) unvisitedCustomersCount / (float) this->subClusterSize);
    // std::cout << "clusterCount " << this->clusterSize << "\n";
    // std::cout << "this->clusters\n";
    // std::cout << subClusterCount << '\n';
    // int** cluster = (int**) initialize(clusterSize, sizeof(int*));

    // int* subCluster = (int*) initialize(this->subClusterSize, sizeof(int));
    // std::cout << "subCluster -- clusters " << clusterIndex << '\n';
    int closestCustomerIndex;

    for(int sectionIndex = 0; sectionIndex < frame.sectorsCount; sectionIndex++) {
        closestCustomerIndex = findClosestNeighborInSector(problemInstance, this->vertexIndex, sectionIndex, visitedCustomersIndexes, frame.customerSectorMap);
        
        if(closestCustomerIndex != -1) {
            std::cout << "closestCustomerIndex " << closestCustomerIndex << '\n';
            this->clusters[0][sectionIndex] = closestCustomerIndex;
            visitedCustomersIndexes[closestCustomerIndex] = 1; //visited
            unvisitedCustomersCount--;
        }
    }


    std::cout << "this->neighborCustomerCount - unvisitedCustomersCount " << this->neighborCustomerCount - unvisitedCustomersCount << "\n";
    std::cout << "frame.sectorsCount " << frame.sectorsCount << "\n";
    for(int subClusterIndex = this->neighborCustomerCount - unvisitedCustomersCount; subClusterIndex < this->subClusterSize; subClusterIndex++) {
        closestCustomerIndex = findClosestNeighbor(problemInstance, this->vertexIndex, visitedCustomersIndexes);
        this->clusters[0][subClusterIndex] = closestCustomerIndex;
        visitedCustomersIndexes[closestCustomerIndex] = 1; //visited
        unvisitedCustomersCount--;
    }

    // for(int subClusterIndex = 0; subClusterIndex < this->subClusterSize; subClusterIndex++) {
    //     int customerIndex = subCluster[subClusterIndex];
    //     int neighborDistance = problemInstance.distanceMatrix[this->vertexIndex][customerIndex];
    //     int neighborSector = frame.customerSectorMap[customerIndex];

    //     std::cout << "--------\n";
    //     std::cout << "SubCluster " << subClusterIndex << "\n";
    //     std::cout << "Neighbor " << customerIndex << "\n";
    //     std::cout << "Distance " << neighborDistance << "\n";
    //     std::cout << "Sector " << neighborSector << "\n";
    //     std::cout << "--------\n";
    // }

    // this->cluster[clusterIndex] = subCluster;
    // clusterIndex++;
    

    // findClosestNeighbor(problemInstance, vertexIndex, visitedCustomersIndexes, customerSectorMap);

    int clusterIndex = 1;
    int subClusterIndex = 0;
    std::cout << "subClusterSize " << subClusterSize << '\n';
    std::cout << "subClusterIndex " << subClusterIndex << '\n';
    while(unvisitedCustomersCount > 0) {
        std::cout << "clusterIndex " << clusterIndex << '\n';
        std::cout << "subClusterIndex " << subClusterIndex << '\n';
        std::cout << "unvisitedCustomersCount " << unvisitedCustomersCount << '\n';
        if(subClusterIndex == subClusterSize) {

            // for(int customerIndex = 0; customerIndex < problemInstance.customerCount; customerIndex++) {
            //     if(visitedCustomersIndexes[customerIndex] == 0) {
            //         std::cout << customerIndex << ' ';
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

        closestCustomerIndex = findClosestNeighbor(problemInstance, this->vertexIndex, visitedCustomersIndexes);
        this->clusters[clusterIndex][subClusterIndex] = closestCustomerIndex;
        visitedCustomersIndexes[closestCustomerIndex] = 1; //visited
        
        subClusterIndex++;
        unvisitedCustomersCount--;
    }

    free(visitedCustomersIndexes);

    // std::cout << closestVertexIndex << '\n';


    return; 
}

void CustomerCluster::finalize() {
    if(this->clusters != nullptr) {
        freeMatrix((void**) this->clusters, this->clusterSize);
    }
}