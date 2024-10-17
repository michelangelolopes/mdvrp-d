#include "../../include/clustering/CustomerCluster.h"

#include <cmath>
#include <iostream>

#include "../../include/clustering/Frame.h"
#include "../../include/utils/ArrayUtils.h"
#include "../../include/utils/MathUtils.h"

void CustomerCluster::create(ProblemInstance problemInstance, Frame frame) {
    
    int* visitedCustomersIndexes = (int*) calloc(problemInstance.customerCount, sizeof(int));

    this->neighborCustomerCount = problemInstance.customerCount;
    this->isCustomerVertex = (this->vertexIndex < problemInstance.customerCount);
    if(this->isCustomerVertex) {
        this->neighborCustomerCount--; //customer vertex does not consider itself
        visitedCustomersIndexes[this->vertexIndex] = 1; //mark itself as visited
    }
    this->clusterSize = std::ceil((float) this->neighborCustomerCount / (float) this->subClusterSize);
    this->clusters = (int**) callocMatrix(this->clusterSize, this->subClusterSize, sizeof(int*), sizeof(int));
    
    int unvisitedCustomersCount = this->neighborCustomerCount;
    int closestCustomerIndex;

    for(int sectionIndex = 0; sectionIndex < frame.sectorsCount; sectionIndex++) {
        closestCustomerIndex = findClosestCustomerInSector(problemInstance, this->vertexIndex, sectionIndex, visitedCustomersIndexes, frame.customerSectorMap);
        
        if(closestCustomerIndex != -1) {
            this->clusters[0][sectionIndex] = closestCustomerIndex;
            visitedCustomersIndexes[closestCustomerIndex] = 1; //visited
            unvisitedCustomersCount--;
        }
    }

    for(int subClusterIndex = this->neighborCustomerCount - unvisitedCustomersCount; subClusterIndex < this->subClusterSize; subClusterIndex++) {
        closestCustomerIndex = findClosestCustomer(problemInstance, this->vertexIndex, visitedCustomersIndexes);
        this->clusters[0][subClusterIndex] = closestCustomerIndex;
        visitedCustomersIndexes[closestCustomerIndex] = 1; //visited
        unvisitedCustomersCount--;
    }

    int clusterIndex = 1;
    int subClusterIndex = 0;
    while(unvisitedCustomersCount > 0) {
        if(subClusterIndex == subClusterSize) {
            subClusterIndex = 0;
            clusterIndex++;
        }

        closestCustomerIndex = findClosestCustomer(problemInstance, this->vertexIndex, visitedCustomersIndexes);
        this->clusters[clusterIndex][subClusterIndex] = closestCustomerIndex;
        visitedCustomersIndexes[closestCustomerIndex] = 1; //visited
        
        subClusterIndex++;
        unvisitedCustomersCount--;
    }

    while(subClusterIndex < subClusterSize) {
        this->clusters[clusterIndex][subClusterIndex] = -1;
        subClusterIndex++;
    }

    free(visitedCustomersIndexes);
}

void CustomerCluster::finalize() {
    if(this->clusters != nullptr) {
        freeMatrix((void**) this->clusters, this->clusterSize);
    }
}

void CustomerCluster::print() {

    std::cout << "\n--------------------------------------------------\n";

    std::cout << "CustomerCluster - ";
    if(isCustomerVertex) {
        std::cout << "Customer[" << vertexIndex << "] - ";
    } else {
        std::cout << "Depot[" << vertexIndex - neighborCustomerCount << "] - ";
    }

    std::cout << "SubClusterSize: " << subClusterSize << '\n';

    std::cout << "-------------------------\n";

    for(int clusterIndex = 0; clusterIndex < clusterSize; clusterIndex++) {
        std::cout << "Cluster[" << clusterIndex << "]: ";
        for(int subClusterIndex = 0; subClusterIndex < this->subClusterSize; subClusterIndex++) {
            std::cout << clusters[clusterIndex][subClusterIndex] << "\t";
        }
        std::cout << '\n';
    }

    std::cout << "--------------------------------------------------\n";
}

int findClosestCustomer(ProblemInstance problemInstance, int vertexIndex, int* visitedCustomersIndexes) {

    int closestCustomerIndex = -1;
    double closestCustomerDistance = -1;
    for(int customerIndex = 0; customerIndex < problemInstance.customerCount; customerIndex++) {
        if(visitedCustomersIndexes[customerIndex] != 1) {
            double neighborCustomerDistance = problemInstance.distanceMatrix[vertexIndex][customerIndex];

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestCustomerIndex = customerIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestCustomerIndex;
}

int findClosestCustomerInSector(ProblemInstance problemInstance, int vertexIndex, int currentSectorIndex, int* visitedCustomersIndexes, int* customerSectorMap) {

    int closestCustomerIndex = -1;
    double closestCustomerDistance = -1;
    for(int customerIndex = 0; customerIndex < problemInstance.customerCount; customerIndex++) {
        if(visitedCustomersIndexes[customerIndex] != 1 && customerSectorMap[customerIndex] == currentSectorIndex) {
            double neighborCustomerDistance = problemInstance.distanceMatrix[vertexIndex][customerIndex];

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestCustomerIndex = customerIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestCustomerIndex;
}