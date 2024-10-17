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
    
    problemInstance.finalize();

    if(bestSolution != nullptr) {
        bestSolution->finalize();
        delete bestSolution;
    }

    frame.finalize();

    if(this->customerClusters != nullptr) {
        for(int vertexIndex = 0; vertexIndex < this->problemInstance.vertexCount; vertexIndex++) {
            customerClusters[vertexIndex].finalize();
        }
        free(this->customerClusters);
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

        for(int vertexIndex = 0; vertexIndex < this->problemInstance.vertexCount; vertexIndex++) {
            for(int customerIndex = 0; customerIndex < problemInstance.customerCount; customerIndex++) {
                this->pheromoneMatrix[depotIndex][vertexIndex][customerIndex] = 1;
            }
        }
    }
}

void StodolaInspiredAntSystem::updatePheromoneMatrix(Solution* consideredSolution) {
    
    //obs: checking precedence is not needed, because only the pheromone of visitedVertices from the route will be updated
    for(int depotIndex = 0; depotIndex < consideredSolution->routesCount; depotIndex++) {
        for(int routeIndex = 0; routeIndex < consideredSolution->routes[depotIndex].routeRealLength - 1; routeIndex++) {
            int vertexIndex = consideredSolution->routes[depotIndex].visitedVertices[routeIndex];
            int neighborVertexIndex = consideredSolution->routes[depotIndex].visitedVertices[routeIndex] + 1;
            
            if(neighborVertexIndex >= problemInstance.customerCount) {//neighbor vertex is a depot
                continue;
            }

            double pheromoneUpdateSum = pheromoneUpdateCoef * (bestSolution->fitness / consideredSolution->fitness);
            pheromoneMatrix[depotIndex][vertexIndex][neighborVertexIndex] += pheromoneUpdateSum;
        }
    }
    
    temperatureUpdateCoef *= temperatureCoolingCoef;
}

void StodolaInspiredAntSystem::updatePheromoneMatrixWithProbability(Solution* generationBestSolution) {
    double bestSolutionProbability = generationBestSolution->fitness - bestSolution->fitness;
    bestSolutionProbability /= bestSolution->fitness;
    bestSolutionProbability /= temperatureUpdateCoef;
    bestSolutionProbability *= -1;
    bestSolutionProbability = exp(bestSolutionProbability);

    double generationBestSolutionProbability = 1 - bestSolutionProbability;
    double randomValue = ((double)rand() / RAND_MAX);

    if(randomValue <= generationBestSolutionProbability) {
        updatePheromoneMatrix(generationBestSolution);
    } else {
        updatePheromoneMatrix(bestSolution);
    }
}

void StodolaInspiredAntSystem::updateEvaporationCoef(int** generationEdgesOcurrenceSum, int generationEdgesSum) {
    double informationEntropy = calculateInformationEntropy(generationEdgesOcurrenceSum, generationEdgesSum);
}

void StodolaInspiredAntSystem::run() {

    srand((unsigned int)time(0));

    int maxIter = 1000;
    int iterIndex = 0;
    while(iterIndex < maxIter) { //TODO: adjust termination condition
        Solution* generationBestSolution = nullptr;
        int generationEdgesSum = 0;
        int** generationEdgesOcurrenceSum = (int**) initialize(problemInstance.vertexCount, problemInstance.customerCount, sizeof(int*), sizeof(int));
        for(int antIndex = 0; antIndex < this->antsCount; antIndex++) {
            Solution antSolution = buildAntSolution();

            for(int depotIndex = 0; depotIndex < antSolution.routesCount; depotIndex++) {
                for(int routeIndex = 0; routeIndex < antSolution.routes[depotIndex].routeRealLength - 1; routeIndex++) {
                    int vertexIndex = antSolution.routes[depotIndex].visitedVertices[routeIndex];
                    int neighborVertexIndex = antSolution.routes[depotIndex].visitedVertices[routeIndex] + 1;
                    
                    if(neighborVertexIndex >= problemInstance.customerCount) {//neighbor vertex is a depot
                        continue;
                    }

                    generationEdgesOcurrenceSum[vertexIndex][neighborVertexIndex] += 1;
                    generationEdgesSum += 1;
                }
            }

            if(generationBestSolution == nullptr) {
                generationBestSolution = new Solution(antSolution);
            } else if(antSolution.fitness <= generationBestSolution->fitness) {
                generationBestSolution->finalize();
                delete generationBestSolution;

                generationBestSolution = new Solution(antSolution);
            }
            antSolution.finalize();
        }

        if(bestSolution == nullptr || generationBestSolution->fitness <= bestSolution->fitness) {
            std::cout << "generation " << iterIndex << " - best solution\n";
            bestSolution = generationBestSolution;
            bestSolution->print();

            updatePheromoneMatrix(bestSolution);
        } else if(generationBestSolution->fitness > bestSolution->fitness){
            updatePheromoneMatrixWithProbability(generationBestSolution);

            generationBestSolution->finalize();
            delete generationBestSolution;
        }

        updateEvaporationCoef(generationEdgesOcurrenceSum, generationEdgesSum);

        iterIndex += 1;
        freeMatrix((void**) generationEdgesOcurrenceSum, problemInstance.vertexCount);
    }
}

Solution StodolaInspiredAntSystem::buildAntSolution() {

    int* visitedCustomersIndexes = (int*) initialize(problemInstance.customerCount, sizeof(int));
    int* currentVertexIndex = (int*) initialize(problemInstance.depotCount, sizeof(int));
    int* currentRouteIndex = (int*) initialize(problemInstance.depotCount, sizeof(int));
    double* currentTruckLoad = (double*) initialize(problemInstance.depotCount, sizeof(double));

    int unvisitedCustomersCount = problemInstance.customerCount;

    int routeMaxLength = (problemInstance.customerCount * 2) + 1; //route worst case: (depot, customer), (depot, customer), ..., depot
    Solution antSolution(problemInstance.depotCount, routeMaxLength);

    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        int vertexIndex = depotIndex + problemInstance.customerCount;
        antSolution.routes[depotIndex].visitedVertices[0] = vertexIndex; //first vertex visited
        currentVertexIndex[depotIndex] = vertexIndex;
        currentRouteIndex[depotIndex] = 0;
        currentTruckLoad[depotIndex] = 0;
    }

    int depotIndex = 0; //TODO: check if the initial depot can be selected randomly
    while(unvisitedCustomersCount > 0) {
        depotIndex = selectDepot(currentVertexIndex[depotIndex], visitedCustomersIndexes);
        int clusterIndex = selectCluster(currentVertexIndex[depotIndex], visitedCustomersIndexes, depotIndex);
        int customerIndex = selectCustomer(currentVertexIndex[depotIndex], visitedCustomersIndexes, depotIndex, clusterIndex);

        if(currentTruckLoad[depotIndex] + problemInstance.customers[customerIndex].demand > problemInstance.depots[depotIndex].trucks[0].capacity) {
            int vertexIndex = depotIndex + problemInstance.customerCount;
            currentRouteIndex[depotIndex]++;
            antSolution.routes[depotIndex].visitedVertices[currentRouteIndex[depotIndex]] = vertexIndex;
            currentTruckLoad[depotIndex] = 0;
        }

        currentRouteIndex[depotIndex]++;
        antSolution.routes[depotIndex].visitedVertices[currentRouteIndex[depotIndex]] = customerIndex;
        currentTruckLoad[depotIndex] += problemInstance.customers[customerIndex].demand;
        visitedCustomersIndexes[customerIndex] = 1;
        currentVertexIndex[depotIndex] = customerIndex;

        unvisitedCustomersCount--;
    }

    for(int vertexIndex = problemInstance.customerCount; vertexIndex < problemInstance.vertexCount; vertexIndex++) {
        int depotIndex = vertexIndex - problemInstance.customerCount;
        currentRouteIndex[depotIndex]++;
        antSolution.routes[depotIndex].visitedVertices[currentRouteIndex[depotIndex]] = vertexIndex;
        antSolution.routes[depotIndex].routeRealLength = currentRouteIndex[depotIndex];
    }

    antSolution.calculateFitness(problemInstance);

    free(visitedCustomersIndexes);
    free(currentVertexIndex);
    free(currentRouteIndex);
    free(currentTruckLoad);

    return antSolution;
}

int StodolaInspiredAntSystem::selectDepot(int vertexIndex, int* visitedCustomersIndexes) {

    double* depotSelectionProbability = (double*) initialize(problemInstance.depotCount, sizeof(double));

    int unvisitedCustomersCountSum = 0;
    for(int depotIndex = 0; depotIndex < problemInstance.depotCount; depotIndex++) {
        for(int clusterIndex = 0; clusterIndex < this->primaryClustersCount; clusterIndex++) {
            for(int subClusterIndex = 0; subClusterIndex < this->customerClusters[vertexIndex].subClusterSize; subClusterIndex++) {
                int customerIndex = this->customerClusters[vertexIndex].clusters[clusterIndex][subClusterIndex];

                if(visitedCustomersIndexes[customerIndex] != 1) {
                    unvisitedCustomersCountSum++;
                    depotSelectionProbability[depotIndex] += this->pheromoneMatrix[depotIndex][vertexIndex][customerIndex];
                }
            }
        }
    }

    int selectedDepotIndex = -1;
    if(unvisitedCustomersCountSum > 0) {
        selectedDepotIndex = rouletteWheelSelection(
            this->problemInstance.depotCount,
            depotSelectionProbability
        );
    } else { //TODO: review this temporary logic to consider non primary clusters
        selectedDepotIndex = ((int)rand() / RAND_MAX) * problemInstance.depotCount;
    }

    free(depotSelectionProbability);

    return selectedDepotIndex;
}

int StodolaInspiredAntSystem::selectCluster(int vertexIndex, int* visitedCustomersIndexes, int depotIndex) {

    double* heuristicInformationAverage = (double*) initialize(this->primaryClustersCount, sizeof(double));
    double* pheromoneConcentrationAverage = (double*) initialize(this->primaryClustersCount, sizeof(double));
    double* clusterSelectionProbability = (double*) initialize(problemInstance.depotCount, sizeof(double));

    int unvisitedCustomersCountSum = 0;
    int selectedClusterIndex = -1;

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
        } else { //TODO: use a fill with zeros method
            heuristicInformationAverage[clusterIndex] = 0;
            pheromoneConcentrationAverage[clusterIndex] = 0;
        }
    }

    if(unvisitedCustomersCountSum > 0) {
        for(int clusterIndex = 0; clusterIndex < this->primaryClustersCount; clusterIndex++) {
            clusterSelectionProbability[clusterIndex] = 1;
            clusterSelectionProbability[clusterIndex] *= pow(heuristicInformationAverage[clusterIndex], this->distanceProbabilityCoef);
            clusterSelectionProbability[clusterIndex] *= pow(pheromoneConcentrationAverage[clusterIndex], this->pheromoneProbabilityCoef);
        }

        selectedClusterIndex = rouletteWheelSelection(
            this->primaryClustersCount,
            clusterSelectionProbability
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

            if(customerIndex == -1) { //the last cluster can have -1 elements if its not fully filled
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

            if(customerIndex == -1) { //the last cluster can have -1 elements if its not fully filled
                break;
            }

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
        customerSelectionProbability
    );

    return this->customerClusters[vertexIndex].clusters[clusterIndex][subClusterIndex];
}

double StodolaInspiredAntSystem::calculateInformationEntropy(int** generationEdgesOcurrenceSum, int generationEdgesSum) {
    double informationEntropy = 0;

    for(int vertexIndex = 0; vertexIndex < problemInstance.vertexCount; vertexIndex++) {
        for(int customerIndex = 0; customerIndex < problemInstance.customerCount; customerIndex++) {
            double edgeOcurrenceProbability = generationEdgesOcurrenceSum[vertexIndex][customerIndex];

            if(edgeOcurrenceProbability > 0) {
                edgeOcurrenceProbability /= generationEdgesSum;
                informationEntropy += (edgeOcurrenceProbability * log(edgeOcurrenceProbability));
            }
        }
    }

    informationEntropy *= -1;

    return informationEntropy;
}

void normalizeValues(int candidatesCount, double* selectionProbability) {
    double cumulativeProbability = 0;
    for(int candidateIndex = 0; candidateIndex < candidatesCount; candidateIndex++) {
        cumulativeProbability += selectionProbability[candidateIndex];
    }
    for(int candidateIndex = 0; candidateIndex < candidatesCount; candidateIndex++) {
        selectionProbability[candidateIndex] /= cumulativeProbability;
    }
}

int rouletteWheelSelection(int candidatesCount, double* selectionProbability) {
    
    normalizeValues(candidatesCount, selectionProbability);
    
    double randomValue = ((double)rand() / RAND_MAX);
    
    double cumulativeProbability = 0;
    for(int candidateIndex = 0; candidateIndex < candidatesCount; candidateIndex++) {
        cumulativeProbability += selectionProbability[candidateIndex];
        if (randomValue <= cumulativeProbability) {
            return candidateIndex;
        }
    }

    return -1;
}