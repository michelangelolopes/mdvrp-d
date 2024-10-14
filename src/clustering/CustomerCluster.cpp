#include "../../include/clustering/Cluster.h"

#include <cmath>
#include <iostream>

#include "../../include/utils/ArrayUtils.h"
#include "../../include/utils/MathUtils.h"

int findClosestCustomerInSector(Problem problem, int currentCustomerIndex, int currentSectorIndex, int* freeCustomersIndexes, int* customerSectorMap) {

    int closestCustomerIndex = -1;
    double closestCustomerDistance = -1;
    for(int customerIndex = 0; customerIndex < problem.customerCount; customerIndex++) {
        if(freeCustomersIndexes[customerIndex] != 1 && customerSectorMap[customerIndex] == currentSectorIndex) {
            double neighborCustomerDistance = calculateEuclidianDistance(
                problem.customers[currentCustomerIndex], 
                problem.customers[customerIndex]
            );

            // std::cout << neighborCustomerDistance << '\n';

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestCustomerIndex = customerIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestCustomerIndex;
}

int findClosestCustomer(Problem problem, int currentCustomerIndex, int* freeCustomersIndexes) {

    int closestCustomerIndex = -1;
    double closestCustomerDistance = -1;
    for(int neighborIndex = 0; neighborIndex < problem.customerCount; neighborIndex++) {
        if(freeCustomersIndexes[neighborIndex] != 1) {
            double neighborCustomerDistance = problem.customerDistanceMatrix[currentCustomerIndex][neighborIndex];

            // std::cout << neighborCustomerDistance << '\n';

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestCustomerIndex = neighborIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestCustomerIndex;
}

void createCluster(Problem problem, int currentCustomerIndex, int subClusterSize, int clusterSectionCount) {

    int clusterSize = std::ceil((float) problem.customerCount / (float) subClusterSize);
    // std::cout << subClusterCount << '\n';
    int** cluster = (int**) initialize(clusterSize, sizeof(int*));
    int* freeCustomersIndexes = (int*) initialize(problem.customerCount, sizeof(int));
    freeCustomersIndexes[currentCustomerIndex] = 1; //cluster vertex should not be considered

    // for(int sectionIndex = 0; sectionIndex < clusterSectionCount; sectionIndex++) {
    // }
    
    int freeCustomersCount = problem.customerCount - 1;
    int clusterIndex = 0;
    int subClusterCount = 0;
    int* subCluster = (int*) initialize(subClusterSize, sizeof(int));

    // findClosestCustomer(problem, currentCustomerIndex, freeCustomersIndexes, customerSectorMap);

    while(freeCustomersCount > 0) {
        
        if(subClusterCount >= subClusterSize) {
            subClusterCount = 0;
            cluster[clusterIndex] = subCluster;
            clusterIndex++;
            subCluster = (int*) initialize(subClusterSize, sizeof(int));
        }

        subCluster[subClusterCount] = findClosestCustomer(problem, currentCustomerIndex, freeCustomersIndexes);
        
        subClusterCount++;
        freeCustomersCount--;
    }

    // std::cout << closestCustomerIndex << '\n';


    return; 
}