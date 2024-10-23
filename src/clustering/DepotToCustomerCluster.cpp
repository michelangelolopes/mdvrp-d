#include "../../include/clustering/DepotToCustomerCluster.hpp"

int DepotToCustomerCluster::findClosestCustomer(ProblemInstance problemInstance, int depotIndex, int* consideredCustomersIndexes) {

    int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);

    int memberIndex = -1;
    double minDistance = -1;

    for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {

        if(consideredCustomersIndexes[customerIndex] != 1) {

            double distance = problemInstance.verticesDistanceMatrix[depotVertexIndex][customerIndex];

            if(minDistance == -1 || minDistance > distance) {
                memberIndex = customerIndex;
                minDistance = distance;
            }
        }
    }

    // if(depotIndex == 0) {

    //     std::cout << "[" << depotIndex << "]";
    //     std::cout << "[" << memberIndex << "] = ";
    //     std::cout << minDistance << "\n";
    // }

    return memberIndex;
}

int DepotToCustomerCluster::findClosestCustomerInSector(ProblemInstance problemInstance, int depotIndex, int sectorIndex, int* consideredCustomersIndexes, int* customerSectorMap) {

    int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);

    int memberIndex = -1;
    double minDistance = -1;

    for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {

        if(consideredCustomersIndexes[customerIndex] != 1 && customerSectorMap[customerIndex] == sectorIndex) {

            double distance = problemInstance.verticesDistanceMatrix[depotVertexIndex][customerIndex];

            if(minDistance == -1 || minDistance > distance) {
                memberIndex = customerIndex;
                minDistance = distance;
            }
        }
    }

    // if(depotIndex == 0) {
    //     std::cout << "Sectors[" << sectorIndex << "]: ";
    //     std::cout << "[" << depotIndex << "]";
    //     std::cout << "[" << memberIndex << "] = ";
    //     std::cout << minDistance << "\n";
    // }

    return memberIndex;
}