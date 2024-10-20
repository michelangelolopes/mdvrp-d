#include "../../include/clustering/CustomerToCustomerCluster.h"

int CustomerToCustomerCluster::findClosestCustomer(ProblemInstance problemInstance, int customerIndex, int* consideredCustomersIndexes) {

    int memberIndex = -1;
    double minDistance = -1;

    for(int neighborCustomerIndex = 0; neighborCustomerIndex < problemInstance.customersCount; neighborCustomerIndex++) {

        if(consideredCustomersIndexes[neighborCustomerIndex] != 1) {

            double distance = problemInstance.customerToCustomerDistanceMatrix[customerIndex][neighborCustomerIndex];

            if(minDistance == -1 || minDistance > distance) {
                memberIndex = neighborCustomerIndex;
                minDistance = distance;
            }
        }
    }

    return memberIndex;
}

int CustomerToCustomerCluster::findClosestCustomerInSector(ProblemInstance problemInstance, int customerIndex, int sectorIndex, int* consideredCustomersIndexes, int* customerSectorMap) {

    int memberIndex = -1;
    double minDistance = -1;

    for(int neighborCustomerIndex = 0; neighborCustomerIndex < problemInstance.customersCount; neighborCustomerIndex++) {

        if(consideredCustomersIndexes[neighborCustomerIndex] != 1 && customerSectorMap[neighborCustomerIndex] == sectorIndex) {

            double distance = problemInstance.customerToCustomerDistanceMatrix[customerIndex][neighborCustomerIndex];

            if(minDistance == -1 || minDistance > distance) {
                memberIndex = neighborCustomerIndex;
                minDistance = distance;
            }
        }
    }

    return memberIndex;
}