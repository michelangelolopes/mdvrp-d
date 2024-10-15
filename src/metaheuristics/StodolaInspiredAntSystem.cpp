#include "../../include/metaheuristics/StodolaInspiredAntSystem.h"

#include <cmath>    // ceil()
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <iostream>

#include "../../include/utils/ArrayUtils.h"

void StodolaInspiredAntSystem::createFrame(int sectorsCount) {
    this->frame = Frame(this->problemInstance, sectorsCount);
}

void StodolaInspiredAntSystem::createCustomerClusters(int subClusterSize) {
    std::cout << "StodolaInspiredAntSystem::createCustomerClusters() -- start\n";
    customerClusters = (CustomerCluster*) initialize(this->problemInstance.vertexCount, sizeof(CustomerCluster));

    for(int vertexIndex = 0; vertexIndex < this->problemInstance.vertexCount; vertexIndex++) {
        customerClusters[vertexIndex] = CustomerCluster(vertexIndex, subClusterSize);
        customerClusters[vertexIndex].create(this->problemInstance, this->frame);
    }

    std::cout << "StodolaInspiredAntSystem::createCustomerClusters() -- end\n";
}

void StodolaInspiredAntSystem::finalize() {
    
    if(this->customerClusters != nullptr) {
        for(int customerIndex = 0; customerIndex < this->problemInstance.customerCount; customerIndex++) {
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
    int unvisitedCustomersCount = this->problemInstance.customerCount - 1;
    int clusterCount = std::ceil((float) unvisitedCustomersCount / (float) subClusterSize);

    for(int customerIndex = 0; customerIndex < this->problemInstance.customerCount; customerIndex++) {
        std::cout << "--------------------------------\n";
        std::cout << "Cluster for Customer " << customerIndex << "\n";
        for(int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {
            std::cout << "---------------\n";
            std::cout << "Cluster " << clusterIndex << "\n";

            for(int subClusterIndex = 0; subClusterIndex < subClusterSize; subClusterIndex++) {
                int neighborIndex = this->customerClusters[customerIndex].clusters[clusterIndex][subClusterIndex];
                int neighborDistance = this->problemInstance.distanceMatrix[customerIndex][neighborIndex];
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

void StodolaInspiredAntSystem::initializePheromoneMatrix() {
    this->pheromoneMatrix = (double***) initialize(this->problemInstance.depotCount, sizeof(double**));

    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        this->pheromoneMatrix[depotIndex] = (double**) initialize(this->problemInstance.vertexCount, this->problemInstance.customerCount, sizeof(double*), sizeof(double));
    }
}

void StodolaInspiredAntSystem::run() {
    srand((unsigned int)time(0));
    this->antsSolution = (Solution*) initialize(antsCount, sizeof(Solution));

    for(int antIndex = 0; antIndex < this->antsCount; antIndex++) {
        this->antsSolution[antIndex] = buildAntSolution();
        break; //test only one
    }
}

Solution StodolaInspiredAntSystem::buildAntSolution() {
    int* visitedCustomersIndexes = (int*) initialize(problemInstance.customerCount, sizeof(int));
    int* currentVertexIndex = (int*) initialize(problemInstance.customerCount, sizeof(int));
    double* currentTruckLoad = (double*) initialize(problemInstance.customerCount, sizeof(double));

    int unvisitedCustomersCount = problemInstance.customerCount;

    Solution antSolution;
    antSolution.routes = (int**) initialize(problemInstance.depotCount, routeMaxLength, sizeof(int*), sizeof(int));

    for(int depotIndex = problemInstance.customerCount; depotIndex < problemInstance.vertexCount; depotIndex++) {
        antSolution.routes[depotIndex][0] = depotIndex; //first vertex visited
        currentVertexIndex[depotIndex] = depotIndex;
        currentTruckLoad[depotIndex] = 0;
    }

    int depotIndex = 0;
    int clusterIndex = -1;

    while(unvisitedCustomersCount > 0) {
        depotIndex = selectDepot(currentVertexIndex[depotIndex], visitedCustomersIndexes);
        break;
        // clusterIndex = selectCluster();
        // int* candidates;
        // int vertexIndex = selectCustomer();
    }
}

int StodolaInspiredAntSystem::selectDepot(int vertexIndex, int* visitedCustomersIndexes) {
    // this->problemInstance.depots;
    // this->customerClusters;
    // this->pheromoneMatrix;
    // this->primaryClustersCount;
    double* depotSelectionProbability = (double*) initialize(problemInstance.depotCount, sizeof(double));

    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        for(int clusterIndex = 0; clusterIndex < this->primaryClustersCount; clusterIndex++) {
            for(int subClusterIndex = 0; subClusterIndex < this->customerClusters[vertexIndex].subClusterSize; subClusterIndex++) {
                int closestCustomerIndex = this->customerClusters[vertexIndex].clusters[clusterIndex][subClusterIndex];

                if(visitedCustomersIndexes[closestCustomerIndex] != 1) {
                    depotSelectionProbability[depotIndex] += this->pheromoneMatrix[depotIndex][vertexIndex][closestCustomerIndex];
                }
            }
        }

    }

    double selectionProbabilitySum = 0;
    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        selectionProbabilitySum += depotSelectionProbability[depotIndex];
    }

    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        depotSelectionProbability[depotIndex] /= selectionProbabilitySum;
    }

    double randomValue = ((double)rand() / RAND_MAX) * selectionProbabilitySum;
    double cumulativeProbability = 0;
    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        cumulativeProbability += depotSelectionProbability[depotIndex];
        if (randomValue <= cumulativeProbability) {
            return depotIndex;
        }
    }
}