#include "../../include/metaheuristics/StodolaInspiredAntSystem.h"

#include <cmath>    // ceil()
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <iostream>

#include "../../include/utils/ArrayUtils.h"

void StodolaInspiredAntSystem::create(int subClusterSize) {
    initializeCustomerClusters(subClusterSize);
    initializePheromoneMatrix();
}

void StodolaInspiredAntSystem::finalize() {
    
    frame.finalize();

    if(this->customerClusters != nullptr) {
        for(int vertexIndex = 0; vertexIndex < this->problemInstance.vertexCount; vertexIndex++) {
            customerClusters[vertexIndex].finalize();
        }
        free(this->customerClusters);
    }

    if(this->antsSolution != nullptr) {
        for(int antIndex = 0; antIndex < this->antsCount; antIndex++) {
            antsSolution[antIndex].finalize();
        }
        free(this->antsSolution);
    }

    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        freeMatrix((void**) this->pheromoneMatrix[depotIndex], this->problemInstance.vertexCount);
    }

    free(this->pheromoneMatrix);
}

void StodolaInspiredAntSystem::print() {

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

    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        for(int vertexIndex = 0; vertexIndex < this->problemInstance.vertexCount; vertexIndex++) {
            for(int customerIndex = 0; customerIndex < this->problemInstance.customerCount; customerIndex++) {
                std::cout << "[" << depotIndex << "]";
                std::cout << "[" << vertexIndex << "]";
                std::cout << "[" << customerIndex << "]";
                std::cout << " = " << this->pheromoneMatrix[depotIndex][vertexIndex][customerIndex] << '\n';
            }
        }
    }
}

void StodolaInspiredAntSystem::initializeCustomerClusters(int subClusterSize) {

    customerClusters = (CustomerCluster*) initialize(this->problemInstance.vertexCount, sizeof(CustomerCluster));

    for(int vertexIndex = 0; vertexIndex < this->problemInstance.vertexCount; vertexIndex++) {
        customerClusters[vertexIndex] = CustomerCluster(this->problemInstance, this->frame, vertexIndex, subClusterSize);
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
        break; //test
    }
}

Solution StodolaInspiredAntSystem::buildAntSolution() {

    int* visitedCustomersIndexes = (int*) initialize(problemInstance.customerCount, sizeof(int));
    int* currentVertexIndex = (int*) initialize(problemInstance.depotCount, sizeof(int));
    int* currentRouteIndex = (int*) initialize(problemInstance.depotCount, sizeof(int));
    double* currentTruckLoad = (double*) initialize(problemInstance.depotCount, sizeof(double));

    int unvisitedCustomersCount = problemInstance.customerCount;

    Solution antSolution(problemInstance.depotCount, routeMaxLength);

    for(int vertexIndex = problemInstance.customerCount; vertexIndex < problemInstance.vertexCount; vertexIndex++) {
        int depotIndex = vertexIndex - problemInstance.customerCount;
        antSolution.routes[depotIndex][0] = vertexIndex; //first vertex visited
        currentVertexIndex[depotIndex] = vertexIndex;
        currentRouteIndex[depotIndex] = 0;
        currentTruckLoad[depotIndex] = 0;
    }

    int depotIndex = 0; //can select a depot randomly
    while(unvisitedCustomersCount > 0) {
        depotIndex = selectDepot(currentVertexIndex[depotIndex], visitedCustomersIndexes);
        int clusterIndex = selectCluster(currentVertexIndex[depotIndex], visitedCustomersIndexes, depotIndex);
        int customerIndex = selectCustomer(currentVertexIndex[depotIndex], visitedCustomersIndexes, depotIndex, clusterIndex);

        if(currentTruckLoad[depotIndex] + problemInstance.customers[customerIndex].demand > problemInstance.depots[depotIndex].trucks[0].capacity) {
            int vertexIndex = depotIndex + problemInstance.customerCount;
            currentRouteIndex[depotIndex]++;
            antSolution.routes[depotIndex][currentRouteIndex[depotIndex]] = vertexIndex;
            currentTruckLoad[depotIndex] = 0;
        }

        currentRouteIndex[depotIndex]++;
        antSolution.routes[depotIndex][currentRouteIndex[depotIndex]] = customerIndex;
        currentTruckLoad[depotIndex] += problemInstance.customers[customerIndex].demand;
        visitedCustomersIndexes[customerIndex] = 1;
        currentVertexIndex[depotIndex] = customerIndex;
    }

    for(int vertexIndex = problemInstance.customerCount; vertexIndex < problemInstance.vertexCount; vertexIndex++) {
        int depotIndex = vertexIndex - problemInstance.customerCount;
        currentRouteIndex[depotIndex]++;
        antSolution.routes[depotIndex][currentRouteIndex[depotIndex]] = vertexIndex;
    }

    free(visitedCustomersIndexes);
    free(currentVertexIndex);
    free(currentRouteIndex);
    free(currentTruckLoad);

    return antSolution;
}

int StodolaInspiredAntSystem::selectDepot(int vertexIndex, int* visitedCustomersIndexes) {

    double* depotSelectionProbability = (double*) initialize(problemInstance.depotCount, sizeof(double));

    double totalDepotSelectionProbabilitySum = 0;
    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        for(int clusterIndex = 0; clusterIndex < this->primaryClustersCount; clusterIndex++) {
            for(int subClusterIndex = 0; subClusterIndex < this->customerClusters[vertexIndex].subClusterSize; subClusterIndex++) {
                int customerIndex = this->customerClusters[vertexIndex].clusters[clusterIndex][subClusterIndex];

                if(visitedCustomersIndexes[customerIndex] != 1) {
                    depotSelectionProbability[depotIndex] += this->pheromoneMatrix[depotIndex][vertexIndex][customerIndex];
                }
            }
        }
        totalDepotSelectionProbabilitySum += depotSelectionProbability[depotIndex];
    }

    //TODO: consider non primary clusters

    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        depotSelectionProbability[depotIndex] /= totalDepotSelectionProbabilitySum;
    }

    int depotIndex = rouletteWheelSelection(
        this->problemInstance.depotCount,
        depotSelectionProbability,
        totalDepotSelectionProbabilitySum
    );

    free(depotSelectionProbability);

    return depotIndex;
}

int StodolaInspiredAntSystem::selectCluster(int vertexIndex, int* visitedCustomersIndexes, int depotIndex) {

    double* heuristicInformationAverage = (double*) initialize(this->primaryClustersCount, sizeof(double));
    double* pheromoneConcentrationAverage = (double*) initialize(this->primaryClustersCount, sizeof(double));
    double* clusterSelectionProbability = (double*) initialize(problemInstance.depotCount, sizeof(double));

    int unvisitedCustomersCountSum = 0;
    int selectedClusterIndex = 0;

    for(int clusterIndex = 0; clusterIndex < this->primaryClustersCount; clusterIndex++) {
        int unvisitedCustomersCount = 0;
        double heuristicInformationSum = 0;
        double pheromoneConcentrationSum = 0;

        for(int subClusterIndex = 0; subClusterIndex < this->customerClusters[vertexIndex].subClusterSize; subClusterIndex++) {
            int customerIndex = this->customerClusters[vertexIndex].clusters[clusterIndex][subClusterIndex];
            if(visitedCustomersIndexes[customerIndex] != 1) {
                heuristicInformationSum += this->problemInstance.distanceMatrix[vertexIndex][customerIndex];
                pheromoneConcentrationSum += this->pheromoneMatrix[depotIndex][vertexIndex][customerIndex];
                unvisitedCustomersCount++;
            }
        }

        unvisitedCustomersCountSum += unvisitedCustomersCount;

        if(unvisitedCustomersCount > 0) {
            heuristicInformationAverage[clusterIndex] = unvisitedCustomersCount * heuristicInformationSum;
            pheromoneConcentrationAverage[clusterIndex] = pheromoneConcentrationSum / unvisitedCustomersCount;
        }
    }

    if(unvisitedCustomersCountSum >= 0) {
        double totalHeuristicInformationSum = 0;
        double totalPheromoneConcentrationSum = 0;

        for(int clusterIndex = 0; clusterIndex < this->primaryClustersCount; clusterIndex++) {
            totalHeuristicInformationSum += pow(heuristicInformationAverage[clusterIndex], this->distanceProbabilityCoef);
            totalPheromoneConcentrationSum += pow(pheromoneConcentrationAverage[clusterIndex], this->pheromoneProbabilityCoef);
        }

        for(int clusterIndex = 0; clusterIndex < this->primaryClustersCount; clusterIndex++) {
            clusterSelectionProbability[clusterIndex] = 1;
            clusterSelectionProbability[clusterIndex] *= pow(heuristicInformationAverage[clusterIndex], this->distanceProbabilityCoef);
            clusterSelectionProbability[clusterIndex] *= pow(pheromoneConcentrationAverage[clusterIndex], this->pheromoneProbabilityCoef);
            clusterSelectionProbability[clusterIndex] /= (totalHeuristicInformationSum * totalPheromoneConcentrationSum);
        }

        selectedClusterIndex = rouletteWheelSelection(
            this->primaryClustersCount,
            clusterSelectionProbability,
            (totalHeuristicInformationSum * totalPheromoneConcentrationSum)
        );
    } else { //if there is no customer unvisited for all primary clusters, choose non primary cluster
        selectedClusterIndex = selectClusterNonPrimary(vertexIndex, visitedCustomersIndexes);
    }

    free(heuristicInformationAverage);
    free(pheromoneConcentrationAverage);
    free(clusterSelectionProbability);

    return selectedClusterIndex;
}

int StodolaInspiredAntSystem::selectClusterNonPrimary(int vertexIndex, int* visitedCustomersIndexes) {
    for(int clusterIndex = this->primaryClustersCount; clusterIndex < this->customerClusters[vertexIndex].clusterSize; clusterIndex++) {
        int unvisitedCustomersCount = 0;
        for(int subClusterIndex = 0; subClusterIndex < this->customerClusters[vertexIndex].subClusterSize; subClusterIndex++) {
            int customerIndex = this->customerClusters[vertexIndex].clusters[clusterIndex][subClusterIndex];

            if(customerIndex == -1) {
                break;
            }

            if(visitedCustomersIndexes[customerIndex] != 1) {
                unvisitedCustomersCount++;
            }
        }

        if(unvisitedCustomersCount > 0) {
            return clusterIndex;
        }
    }

    return -1;
}

int StodolaInspiredAntSystem::selectCustomer(int vertexIndex, int* visitedCustomersIndexes, int depotIndex, int clusterIndex) {
    
    double* customerSelectionProbability = (double*) initialize(this->customerClusters[vertexIndex].subClusterSize, sizeof(double));
    double totalCustomerSelectionProbabilitySum = 0;
    for(int subClusterIndex = 0; subClusterIndex < this->customerClusters[vertexIndex].subClusterSize; subClusterIndex++) {
            int customerIndex = this->customerClusters[vertexIndex].clusters[clusterIndex][subClusterIndex];

            if(visitedCustomersIndexes[customerIndex] != 1) {
                customerSelectionProbability[subClusterIndex] += pow(this->problemInstance.distanceMatrix[vertexIndex][customerIndex], -this->distanceProbabilityCoef);
                customerSelectionProbability[subClusterIndex] *= pow(this->pheromoneMatrix[depotIndex][vertexIndex][customerIndex], this->pheromoneProbabilityCoef);
                totalCustomerSelectionProbabilitySum += customerSelectionProbability[subClusterIndex];
            }
    }

    for(int subClusterIndex = 0; subClusterIndex < this->customerClusters[vertexIndex].subClusterSize; subClusterIndex++) {
        customerSelectionProbability[subClusterIndex] /= totalCustomerSelectionProbabilitySum;
    }

    int subClusterIndex = rouletteWheelSelection(
        this->customerClusters[vertexIndex].subClusterSize,
        customerSelectionProbability,
        totalCustomerSelectionProbabilitySum
    );

    return this->customerClusters[vertexIndex].clusters[clusterIndex][subClusterIndex];
}

int rouletteWheelSelection(int candidatesCount, double* selectionProbability, double selectionProbabilitySum) {

    double randomValue = ((double)rand() / RAND_MAX) * selectionProbabilitySum;
    double cumulativeProbability = 0;
    for(int candidateIndex = 0; candidateIndex < candidatesCount; candidateIndex++) {
        cumulativeProbability += selectionProbability[candidateIndex];
        if (randomValue <= cumulativeProbability) {
            return candidateIndex;
        }
    }

    return -1;
}