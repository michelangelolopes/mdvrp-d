#include "../../include/metaheuristics/StodolaInspiredAntSystem.h"

#include <cmath>
#include <iostream>

#include "../../include/utils/ArrayUtils.h"

void StodolaInspiredAntSystem::createFrame(int sectorsCount) {
    this->frame = Frame(this->problem, sectorsCount);
}

void StodolaInspiredAntSystem::createCustomerClusters(int subClusterSize) {
    std::cout << "StodolaInspiredAntSystem::createCustomerClusters() -- start\n";
    customerClusters = (CustomerCluster*) initialize(this->problem.customerCount, sizeof(CustomerCluster));

    for(int customerIndex = 0; customerIndex < this->problem.customerCount; customerIndex++) {
        customerClusters[customerIndex] = CustomerCluster(customerIndex, subClusterSize);
        customerClusters[customerIndex].create(this->problem, this->frame);
    }

    std::cout << "StodolaInspiredAntSystem::createCustomerClusters() -- end\n";
}

void StodolaInspiredAntSystem::finalize() {
    
    if(this->customerClusters != nullptr) {
        for(int customerIndex = 0; customerIndex < this->problem.customerCount; customerIndex++) {
            customerClusters[customerIndex].finalize();
            std::cout << "free - customerClusters - " << customerIndex << '\n';
        }
        std::cout << "free - customerClusters - " << this->customerClusters << '\n';
        free(this->customerClusters);
        std::cout << "free - customerClusters" << '\n';
    }
    
    frame.finalize();
}

void StodolaInspiredAntSystem::printCluster() {

    int subClusterSize = this->customerClusters[0].subClusterSize;
    int freeCustomersCount = this->problem.customerCount - 1;
    int clusterCount = std::ceil((float) freeCustomersCount / (float) subClusterSize);

    for(int customerIndex = 0; customerIndex < this->problem.customerCount; customerIndex++) {
        std::cout << "--------------------------------\n";
        std::cout << "Cluster for Customer " << customerIndex << "\n";
        for(int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {
            std::cout << "---------------\n";
            std::cout << "Cluster " << clusterIndex << "\n";

            for(int subClusterIndex = 0; subClusterIndex < subClusterSize; subClusterIndex++) {
                int neighborIndex = this->customerClusters[customerIndex].clusters[clusterIndex][subClusterIndex];
                int neighborDistance = this->problem.customerDistanceMatrix[customerIndex][neighborIndex];
                int neighborSector = this->frame.customerSectorMap[neighborIndex];

                std::cout << "--------\n";
                std::cout << "SubCluster " << subClusterIndex << "\n";
                std::cout << "Neighbor " << neighborIndex << "\n";
                std::cout << "Distance " << neighborDistance << "\n";
                std::cout << "Sector " << neighborSector << "\n";
                std::cout << "--------\n";
            }
            std::cout << "---------------\n";
        }
        std::cout << "--------------------------------\n";
    }
}