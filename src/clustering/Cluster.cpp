#include "../../include/clustering/Cluster.hpp"

#include <cmath>
#include <iostream>

#include "../../include/clustering/Frame.hpp"
#include "../../include/utils/ArrayUtils.hpp"
#include "../../include/utils/MathUtils.hpp"

void Cluster::create(int primarySubClustersMaxCount, int subClusterMaxSize, int vertexIndex) {
    
    int neighborCustomersCount = problemInstance->customersCount;
    int* consideredCustomersIndexes = (int*) calloc(problemInstance->customersCount, sizeof(int));

    if(vertexIndex < problemInstance->customersCount) { //customerVertex
        neighborCustomersCount--;
        consideredCustomersIndexes[vertexIndex] = 1; //mark itself as visited
    }

    initializeSubClusters(primarySubClustersMaxCount, subClusterMaxSize, neighborCustomersCount);
    createFirstSubCluster(vertexIndex, consideredCustomersIndexes);
    createOthersSubClusters(vertexIndex, consideredCustomersIndexes);

    free(consideredCustomersIndexes);
}

void Cluster::initializeSubClusters(int primarySubClustersMaxCount, int subClusterMaxSize, int neighborCustomersCount) {

    int fullSubClustersCount = neighborCustomersCount / subClusterMaxSize;
    int incompleteSubClusterSize = (neighborCustomersCount % subClusterMaxSize); //last subcluster may not be full

    size = fullSubClustersCount + (incompleteSubClusterSize > 0);
    primariesCount = (primarySubClustersMaxCount <= size) ? primarySubClustersMaxCount : size;

    // std::cout << "primarySubClustersMaxCount: " << primarySubClustersMaxCount << "\n";
    // std::cout << "subClusterMaxSize: " << subClusterMaxSize << "\n";
    // std::cout << "neighborCustomersCount: " << neighborCustomersCount << "\n";
    // std::cout << "fullSubClustersCount: " << fullSubClustersCount << "\n";
    // std::cout << "incompleteSubClusterSize: " << incompleteSubClusterSize << "\n";
    // std::cout << "size: " << size << "\n";
    // std::cout << "primariesCount: " << primariesCount << "\n";

    subClusters = (SubCluster*) malloc(size * sizeof(SubCluster));

    for(int subClusterIndex = 0; subClusterIndex < fullSubClustersCount; subClusterIndex++) {
        subClusters[subClusterIndex] = SubCluster(subClusterMaxSize);
    }

    if(fullSubClustersCount < size) {
        subClusters[size - 1] = SubCluster(incompleteSubClusterSize);
    }
}

void Cluster::createFirstSubCluster(int vertexIndex, int* consideredCustomersIndexes) {

    int consideredCustomersCount = 0;

    //fill the first subcluster with the closest customer in each sector
    if(frame->sectorsCount <= subClusters[0].size) {

        for(int sectionIndex = 0; sectionIndex < frame->sectorsCount; sectionIndex++) {
            int closestCustomerIndex = findClosestCustomerInSector(vertexIndex, sectionIndex, consideredCustomersIndexes);
            
            //when the sector is not empty
            if(closestCustomerIndex != -1) {
                consideredCustomersIndexes[closestCustomerIndex] = 1; //visited
                subClusters[0].elements[consideredCustomersCount++] = closestCustomerIndex;
            }
        }
    }

    for(int elementIndex = consideredCustomersCount; elementIndex < subClusters[0].size; elementIndex++) {
        int closestCustomerIndex = findClosestCustomer(vertexIndex, consideredCustomersIndexes);
        consideredCustomersIndexes[closestCustomerIndex] = 1; //visited
        subClusters[0].elements[elementIndex] = closestCustomerIndex;
    }

    // if(vertexIndex == 0) {
    //     for(int elementIndex = 0; elementIndex < subClusters[0].size; elementIndex++) {
    //         std::cout << subClusters[0].elements[elementIndex] << " ";
    //     }
    //     std::cout << "\n";
    // }
}

void Cluster::createOthersSubClusters(int vertexIndex, int* consideredCustomersIndexes) {

    int subClusterIndex = 1;
    while(subClusterIndex < size) {

        int elementIndex = 0;
        while(elementIndex < subClusters[subClusterIndex].size) {

            int closestCustomerIndex = findClosestCustomer(vertexIndex, consideredCustomersIndexes);

            subClusters[subClusterIndex].elements[elementIndex++] = closestCustomerIndex;
            consideredCustomersIndexes[closestCustomerIndex] = 1; //visited
        }

        subClusterIndex++;
    }

    // if(vertexIndex == 0) {
    //     for(int subClusterIndex = 0; subClusterIndex < size; subClusterIndex++) {
    //         for(int elementIndex = 0; elementIndex < subClusters[subClusterIndex].size; elementIndex++) {
    //             std::cout << subClusters[subClusterIndex].elements[elementIndex] << " ";
    //         }
    //         std::cout << "\n";
    //     }
    // }
}

void Cluster::finalize() {

    if(subClusters != nullptr) {

        for(int index = 0; index < size; index++) {
            subClusters[index].finalize();
        }

        free(subClusters);
    }
}

void Cluster::print() {

    // std::cout << "--------------------------------------------------\n";

    // for(int subClusterIndex = 0; subClusterIndex < size; subClusterIndex++) {

    //     std::cout << "SubCluster[" << subClusterIndex << "]: ";
    //     subClusters[subClusterIndex].print();
    //     std::cout << '\n';
    // }

    // std::cout << "--------------------------------------------------\n";
    for(int subClusterIndex = 0; subClusterIndex < size; subClusterIndex++) {

        std::cout << "| ";
        subClusters[subClusterIndex].print();
        std::cout << " ";
    }

    std::cout << "|\n";
}

void Cluster::print(int* visitedCustomersIndexes) {

    // std::cout << "--------------------------------------------------\n";

    // for(int subClusterIndex = 0; subClusterIndex < size; subClusterIndex++) {

    //     std::cout << "SubCluster[" << subClusterIndex << "]: ";
    //     subClusters[subClusterIndex].print(consideredCustomersIndexes);
    //     std::cout << '\n';
    // }

    // std::cout << "--------------------------------------------------\n";
    // std::cout << "--------------------Size:" << size << "-------------------------\n";

    for(int subClusterIndex = 0; subClusterIndex < size; subClusterIndex++) {

        std::cout << "| ";
        subClusters[subClusterIndex].print(visitedCustomersIndexes);
        std::cout << " ";
    }

    std::cout << "|\n";



    // std::cout << "--------------------------------------------------\n";       
}

int Cluster::findClosestCustomer(int vertexIndex, int* consideredCustomersIndexes) {

    int memberIndex = -1;
    double minDistance = -1;

    for(int neighborCustomerIndex = 0; neighborCustomerIndex < problemInstance->customersCount; neighborCustomerIndex++) {

        if(consideredCustomersIndexes[neighborCustomerIndex] != 1) {

            double distance = problemInstance->verticesDistanceMatrix[vertexIndex][neighborCustomerIndex];

            if(minDistance == -1 || minDistance > distance) {
                memberIndex = neighborCustomerIndex;
                minDistance = distance;
            }
        }
    }

    return memberIndex;
}

int Cluster::findClosestCustomerInSector(int vertexIndex, int sectorIndex, int* consideredCustomersIndexes) {

    int memberIndex = -1;
    double minDistance = -1;

    for(int neighborCustomerIndex = 0; neighborCustomerIndex < problemInstance->customersCount; neighborCustomerIndex++) {

        if(consideredCustomersIndexes[neighborCustomerIndex] != 1 && frame->customerSectorMap[neighborCustomerIndex] == sectorIndex) {

            double distance = problemInstance->verticesDistanceMatrix[vertexIndex][neighborCustomerIndex];

            if(minDistance == -1 || minDistance > distance) {
                memberIndex = neighborCustomerIndex;
                minDistance = distance;
            }
        }
    }

    return memberIndex;
}