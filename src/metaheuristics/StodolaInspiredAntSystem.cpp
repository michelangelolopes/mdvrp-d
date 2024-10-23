#include "../../include/metaheuristics/StodolaInspiredAntSystem.hpp"

#include <chrono>
#include <cmath>    // ceil()
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <functional>
#include <iostream>

#include "../../include/utils/ArrayUtils.hpp"

void StodolaInspiredAntSystem::create(int primarySubClustersMaxCount, int subClusterMaxSize) {
    createClusters(primarySubClustersMaxCount, subClusterMaxSize);
    initializePheromoneMatrices();
}

void StodolaInspiredAntSystem::finalize() {
    
    problemInstance.finalize();

    if(bestSolution != nullptr) {
        bestSolution->finalize();
        free(bestSolution);
    }

    if(pheromoneMatrix != nullptr) {

        for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
            freeMatrix(pheromoneMatrix[depotIndex], problemInstance.customersCount);
        }

        free(pheromoneMatrix);
    }

    frame.finalize();

    if(verticesClusters != nullptr) {

        for(int vertexIndex = 0; vertexIndex < problemInstance.vertexCount; vertexIndex++) {
            verticesClusters[vertexIndex].finalize();
        }

        free(verticesClusters);
    }
}

void StodolaInspiredAntSystem::print() {

    // //std::cout << "Cluster - ";
    // if(isCustomerVertex) {
    //     //std::cout << "Customer[" << baseIndex << "] - ";
    // } else {
    //     //std::cout << "Depot[" << baseIndex - neighborcustomersCount << "] - ";
    // }

    // int subClusterSize = oldClusters[0].subClusterSize;
    // int unvisitedCustomersCount = problemInstance.customersCount - 1;
    // int clusterCount = std::ceil((float) unvisitedCustomersCount / (float) subClusterSize);

    // for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
    //     //std::cout << "--------------------------------\n";
    //     //std::cout << "Cluster for Customer " << customerIndex << "\n";
    //     for(int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {
    //         //std::cout << "---------------\n";
    //         //std::cout << "Cluster " << clusterIndex << "\n";

    //         for(int subClusterIndex = 0; subClusterIndex < subClusterSize; subClusterIndex++) {
    //             int neighborIndex = oldClusters[customerIndex].clusters[clusterIndex][subClusterIndex];
    //             int neighborDistance = problemInstance.distanceMatrix[customerIndex][neighborIndex];
    //             int neighborSector = frame.customerSectorMap[neighborIndex];

    //             //std::cout << "--------\n";
    //             //std::cout << "SubCluster " << subClusterIndex << "\n";
    //             //std::cout << "Neighbor " << neighborIndex << "\n";
    //             //std::cout << "Distance " << neighborDistance << "\n";
    //             //std::cout << "Sector " << neighborSector << "\n";
    //             //std::cout << "--------\n";
    //         }
    //         //std::cout << "---------------\n";
    //     }
    //     //std::cout << "--------------------------------\n";
    // }

    // for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
    //     for(int vertexIndex = 0; vertexIndex < problemInstance.vertexCount; vertexIndex++) {
    //         for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
    //             //std::cout << "[" << depotIndex << "]";
    //             //std::cout << "[" << vertexIndex << "]";
    //             //std::cout << "[" << customerIndex << "]";
    //             //std::cout << " = " << pheromoneMatrix[depotIndex][vertexIndex][customerIndex] << '\n';
    //         }
    //     }
    // }
}

void StodolaInspiredAntSystem::createClusters(int primarySubClustersMaxCount, int subClusterMaxSize) {

    verticesClusters = (Cluster*) malloc(problemInstance.vertexCount * sizeof(Cluster));

    for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
        verticesClusters[customerIndex] = Cluster(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, customerIndex);
    }

    for(int depotVertexIndex = problemInstance.getDepotVertexIndex(0); depotVertexIndex < problemInstance.vertexCount; depotVertexIndex++) {
        verticesClusters[depotVertexIndex] = Cluster(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, depotVertexIndex);
    }
}

void StodolaInspiredAntSystem::initializePheromoneMatrices() {

    pheromoneMatrix = (double***) malloc(problemInstance.depotsCount * sizeof(double**));

    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
        pheromoneMatrix[depotIndex] = (double**) mallocMatrix(problemInstance.customersCount, sizeof(double*), sizeof(double));

        for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
            for(int neighborCustomerIndex = 0; neighborCustomerIndex < problemInstance.customersCount; neighborCustomerIndex++) {
                pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex] = 1;
            }
        }

        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);
        for(int neighborCustomerIndex = 0; neighborCustomerIndex < problemInstance.customersCount; neighborCustomerIndex++) {
            pheromoneMatrix[depotIndex][depotVertexIndex][neighborCustomerIndex] = 1;
        }
    }
}

void StodolaInspiredAntSystem::reinforcePheromoneMatrixWithProbability(Solution* generationBestSolution) {

    double bestSolutionProbability = generationBestSolution->fitness - bestSolution->fitness;
    bestSolutionProbability /= bestSolution->fitness;
    bestSolutionProbability /= temperatureUpdateCoef;
    bestSolutionProbability *= -1;
    bestSolutionProbability = exp(bestSolutionProbability);

    double generationBestSolutionProbability = 1 - bestSolutionProbability;
    double randomValue = ((double)rand() / RAND_MAX);

    if(randomValue <= generationBestSolutionProbability) {
        reinforcePheromoneMatrix(generationBestSolution);
    } else {
        reinforcePheromoneMatrix(bestSolution);
    }
}

void StodolaInspiredAntSystem::updatePheromoneMatrix(Solution* consideredSolution, double updateValue, int isSumOperation) {

    std::function<double(double, double)> operationFunction;
    
    if(isSumOperation) {
        operationFunction = [](double x, double y) { return x + y; };
    } else {
        operationFunction = [](double x, double y) { return x * y; };
    }

    //obs: checking precedence is not needed, because only the pheromone of visitedCustomers from the route will be updated
    for(int depotIndex = 0; depotIndex < consideredSolution->depotsCount; depotIndex++) {
        
        Route* route = &consideredSolution->routes[depotIndex];
        for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {
        
            SubRoute* subRoute = &route->subRoutes[subRouteIndex];
            int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);

            int firstCustomerIndex = subRoute->first();
            pheromoneMatrix[depotIndex][depotVertexIndex][firstCustomerIndex] = operationFunction(
                pheromoneMatrix[depotIndex][depotVertexIndex][firstCustomerIndex],
                updateValue
            );

            for(int memberIndex = 0; memberIndex < subRoute->length - 1; memberIndex++) {

                int customerIndex = subRoute->members[memberIndex];
                int neighborCustomerIndex = subRoute->members[memberIndex + 1];
                pheromoneMatrix[depotIndex][depotVertexIndex][firstCustomerIndex] = operationFunction(
                pheromoneMatrix[depotIndex][depotVertexIndex][firstCustomerIndex],
                updateValue
            );
            }

            int lastCustomerIndex = subRoute->last();
            pheromoneMatrix[depotIndex][depotVertexIndex][firstCustomerIndex] = operationFunction(
                pheromoneMatrix[depotIndex][depotVertexIndex][firstCustomerIndex],
                updateValue
            );
        }
    }
}

void StodolaInspiredAntSystem::reinforcePheromoneMatrix(Solution* consideredSolution) {

    double pheromoneReinforcingValue = pheromoneUpdateCoef * (bestSolution->fitness / consideredSolution->fitness);
    int isSumOperation = 1;

    updatePheromoneMatrix(consideredSolution, pheromoneReinforcingValue, isSumOperation);
    
    temperatureUpdateCoef *= temperatureCoolingCoef;
}

void StodolaInspiredAntSystem::evaporatePheromoneMatrix() {

    double pheromoneEvaporatingValue = (1 - pheromoneEvaporationCoef);
    int isSumOperation = 0;

    updatePheromoneMatrix(bestSolution, pheromoneEvaporatingValue, isSumOperation);
}

void StodolaInspiredAntSystem::updateEvaporationCoef(double informationEntropy, double informationEntropyMin, double informationEntropyMax) {

    double relevantInformationEntropyCoef = (informationEntropy - informationEntropyMin) / (informationEntropyMax - informationEntropyMin);

    pheromoneEvaporationCoef = pheromoneEvaporationCoefMin;
    pheromoneEvaporationCoef += (pheromoneEvaporationCoefMax - pheromoneEvaporationCoefMin) * relevantInformationEntropyCoef;
}

int StodolaInspiredAntSystem::hasAchievedTerminationCondition(int iterationsCount, int iterationsWithoutImprovementCount, double currentOptimizationTime, double informationEntropyCoef) {

    // std::cout << "iterationsCount: " << iterationsCount << " - ";
    // std::cout << "iterationsWithoutImprovementCount: " << iterationsWithoutImprovementCount << "\n";
    // std::cout << "currentOptimizationTime: " << currentOptimizationTime << "\n";
    // std::cout << "informationEntropy: " << informationEntropy << " - ";
    // std::cout << "informationEntropyMin: " << informationEntropyMin << " - ";

    // double informationEntropyCoef = (informationEntropy - informationEntropyMin) / informationEntropyMin;
    // std::cout << "informationEntropyCoef: " << informationEntropyCoef << "\n";

    return (iterationsCount >= maxIterations) ||
        (iterationsWithoutImprovementCount >= maxIterationsWithoutImprovement) ||
        (currentOptimizationTime >= maxOptimizationTime) ||
        (!isInformationEntropySufficient(informationEntropyCoef));
}

int StodolaInspiredAntSystem::isInformationEntropySufficient(double informationEntropyCoef) {

    if(informationEntropyCoef == -1) {
        return 1;
    }

    return (informationEntropyCoef >= minInformationEntropyCoef);
}

void StodolaInspiredAntSystem::run() {

    srand((unsigned int)time(0));

    int generationEdgesCount = 0;
    int** generationEdgesOccurrenceCount = (int**) callocMatrix(problemInstance.customersCount, sizeof(int*), sizeof(int));
    int* visitedCustomersIndexes = (int*) malloc(problemInstance.customersCount * sizeof(int));
    
    int globalImprovementsCount = 0;
    int intervalImprovementsCount = 0;

    std::chrono::time_point startOptimizationTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point endOptimizationTime = startOptimizationTime;

    int iterationsCount = 0;
    int iterationsWithoutImprovementCount = 0;
    std::chrono::duration<double> currentOptimizationTime = endOptimizationTime - startOptimizationTime;

    double informationEntropy = -1;
    double informationEntropyMin = -1;
    double informationEntropyMax = -1;
    double informationEntropyCoef = -1;

    //initial solution
    bestSolution = (Solution*) malloc(sizeof(Solution));

    bestSolution[0] = Solution (
        problemInstance.depotsCount,
        problemInstance.minimizationType,
        problemInstance.customersCount
    );

    buildAntRoutes(bestSolution[0], visitedCustomersIndexes);

    Solution antSolution(
        problemInstance.depotsCount,
        problemInstance.minimizationType,
        problemInstance.customersCount
    );

    Solution generationBestSolution = Solution (
        problemInstance.depotsCount,
        problemInstance.minimizationType,
        problemInstance.customersCount
    );

    while(!hasAchievedTerminationCondition(
        iterationsCount, 
        iterationsWithoutImprovementCount,
        currentOptimizationTime.count(),
        informationEntropyCoef
    )) 
    {

        // std::cout << "--- generation: " << iterationsCount << "\n";

        //first ant
        // std::cout << "------ ant: " << 0 << "\n";
        buildAntRoutes(generationBestSolution, visitedCustomersIndexes);
        generationEdgesCount += updateGenerationEdgesOccurrenceCount(generationBestSolution, generationEdgesOccurrenceCount);
        
        //others ants
        for(int antIndex = 1; antIndex < antsCount; antIndex++) {

            // std::cout << "------ ant: " << antIndex << "\n";

            buildAntRoutes(antSolution, visitedCustomersIndexes);
            generationEdgesCount += updateGenerationEdgesOccurrenceCount(antSolution, generationEdgesOccurrenceCount);

            if(antSolution.fitness < generationBestSolution.fitness) {
                generationBestSolution.copy(antSolution);
            }
        }

        //TODO: local optimization
        // if(iterIndex % localOptimizationFrequency == 0) {
        // }

        if(generationBestSolution.fitness < bestSolution->fitness) {

            bestSolution[0].copy(generationBestSolution);
            reinforcePheromoneMatrix(bestSolution);

            intervalImprovementsCount += 1;
            iterationsWithoutImprovementCount = 0;
        } else {
            reinforcePheromoneMatrixWithProbability(&generationBestSolution);
        }

        informationEntropy = calculateInformationEntropy(generationEdgesOccurrenceCount, generationEdgesCount);
        informationEntropyMin = -1 * log((double)antsCount / generationEdgesCount);
        informationEntropyMax = -1 * log(1.00 / generationEdgesCount);
        informationEntropyCoef = (informationEntropy - informationEntropyMin) / informationEntropyMin;

        updateEvaporationCoef(informationEntropy, informationEntropyMin, informationEntropyMax);
        evaporatePheromoneMatrix();

        iterationsCount += 1;
        iterationsWithoutImprovementCount += 1;
        generationEdgesCount = 0;

        fillMatrix(generationEdgesOccurrenceCount, problemInstance.customersCount, 0);

        endOptimizationTime = std::chrono::high_resolution_clock::now();
        currentOptimizationTime = endOptimizationTime - startOptimizationTime;

        if(iterationsCount % 2000 == 0 && intervalImprovementsCount > 0) {

            globalImprovementsCount += intervalImprovementsCount;

            std::cout << "iterationsCount: " << iterationsCount << "\n";
            std::cout << "iterationsWithoutImprovementCount: " << iterationsWithoutImprovementCount << "\n";
            std::cout << "currentOptimizationTime: " << currentOptimizationTime.count() << "\n";
            std::cout << "globalImprovements: " << globalImprovementsCount << " - ";
            std::cout << "intervalImprovements: " << intervalImprovementsCount << "\n";
            std::cout << "informationEntropy: " << informationEntropy << " - ";
            std::cout << "informationEntropyCoef: " << informationEntropyCoef << "\n";
            std::cout << "informationEntropyMin: " << informationEntropyMin << " - ";
            std::cout << "informationEntropyMax: " << informationEntropyMax << "\n";
            
            bestSolution->print();
            
            intervalImprovementsCount = 0;
        }
    }

    std::cout << "iterationsCount: " << iterationsCount << "\n";
    std::cout << "iterationsWithoutImprovementCount: " << iterationsWithoutImprovementCount << "\n";
    std::cout << "currentOptimizationTime: " << currentOptimizationTime.count() << "\n";
    std::cout << "globalImprovements: " << globalImprovementsCount << " - ";
    std::cout << "intervalImprovements: " << intervalImprovementsCount << "\n";
    std::cout << "informationEntropy: " << informationEntropy << " - ";
    std::cout << "informationEntropyCoef: " << informationEntropyCoef << "\n";
    std::cout << "informationEntropyMin: " << informationEntropyMin << " - ";
    std::cout << "informationEntropyMax: " << informationEntropyMax << "\n";
    
    bestSolution->print();

    antSolution.finalize();
    generationBestSolution.finalize();
    
    free(visitedCustomersIndexes);
    freeMatrix(generationEdgesOccurrenceCount, problemInstance.customersCount);
}

void StodolaInspiredAntSystem::buildAntRoutes(Solution& antSolution, int* visitedCustomersIndexes) {

    antSolution.reset();

    int unvisitedCustomersCount = problemInstance.customersCount;
    fillArray(visitedCustomersIndexes, unvisitedCustomersCount, 0);

    while(unvisitedCustomersCount > 0) {

        // std::cout << "--------- unvisitedCustomersCount: " << unvisitedCustomersCount << "\n";
        // std::cout << "--------- unvisitedCustomers: ";
        // printIndexesArray(visitedCustomersIndexes, problemInstance.customersCount, 0);

        int depotIndex = selectDepot(visitedCustomersIndexes, antSolution.routes);
        // std::cout << "------------ depotIndex: " << depotIndex << "\n";

        Route* currentRoute = &antSolution.routes[depotIndex];

        // std::cout << "------------ currentRoute: ";
        // currentRoute->print();
        // std::cout << "\n";

        // int currentVertexIndex = currentRoute->last();
        // std::cout << "------------ currentVertexIndex: " << currentVertexIndex << "\n";
        
        // if(currentVertexIndex != -1) {
        //     std::cout << "------------ currentCluster: ";
        //     customerClusters[currentVertexIndex].print(visitedCustomersIndexes);

        //     std::cout << "------------ fullCluster: ";
        //     customerClusters[currentVertexIndex].print();
        // } else {
        //     std::cout << "------------ currentCluster: ";
        //     depotClusters[depotIndex].print(visitedCustomersIndexes);

        //     std::cout << "------------ fullCluster: ";
        //     depotClusters[depotIndex].print();
        // }

        int subClusterIndex = selectSubCluster(visitedCustomersIndexes, currentRoute->last(), depotIndex);
        // std::cout << "------------ subClusterIndex: " << subClusterIndex << "\n";

        int customerIndex = selectCustomer(visitedCustomersIndexes, currentRoute->last(), depotIndex, subClusterIndex);
        // std::cout << "------------ customerIndex: " << customerIndex << "\n";

        Customer* nextCustomer = &problemInstance.customers[customerIndex];
        Truck* currentTruck = &problemInstance.depots[depotIndex].truck;

        if( ( currentRoute->getCurrentLoad() + nextCustomer->demand ) > currentTruck->capacity ) {
            currentRoute->expand();
        }

        currentRoute->insert(customerIndex);
        currentRoute->incrementCurrentLoad(nextCustomer->demand);

        visitedCustomersIndexes[customerIndex] = 1;
        unvisitedCustomersCount--;
    }

    antSolution.updateFitness(problemInstance);
}

int StodolaInspiredAntSystem::selectDepot(int* visitedCustomersIndexes, Route* routes) {

    double* depotSelectionProbability = (double*) malloc(problemInstance.depotsCount * sizeof(double));
    fillArray(depotSelectionProbability, problemInstance.depotsCount, 0.0);

    int consideredCustomersCount = 0;
    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {

        int vertexIndex = routes[depotIndex].last();
        consideredCustomersCount += updateDepotSelectionProbabilityFromCustomerSource(visitedCustomersIndexes, depotSelectionProbability, depotIndex, vertexIndex);
    }

    int selectedDepotIndex = -1;
    if(consideredCustomersCount > 0) {
        selectedDepotIndex = rouletteWheelSelection(
            depotSelectionProbability,
            problemInstance.depotsCount
        );
    } else { //TODO: review this temporary logic to consider non primary clusters
        selectedDepotIndex = rand() % problemInstance.depotsCount;
    }

    free(depotSelectionProbability);

    return selectedDepotIndex;
}

int StodolaInspiredAntSystem::updateDepotSelectionProbabilityFromCustomerSource(int* visitedCustomersIndexes, double* depotSelectionProbability, int depotIndex, int customerIndex) {
    
    Cluster* cluster = &verticesClusters[customerIndex];
    
    int consideredCustomersCount = 0;
    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {

        SubCluster* subCluster = &cluster->subClusters[subClusterIndex];
        for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

            int neighborCustomerIndex = subCluster->elements[memberIndex];
            if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

                consideredCustomersCount++;
                depotSelectionProbability[depotIndex] += pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex];
            }
        }
    }

    return consideredCustomersCount;
}

int StodolaInspiredAntSystem::selectSubCluster(int* visitedCustomersIndexes, int vertexIndex, int depotIndex) {

    double* primarySubClusterSelectionProbability = nullptr;
    Cluster* cluster = &verticesClusters[vertexIndex];

    primarySubClusterSelectionProbability = getPrimarySubClusterSelectionProbabilityFromCustomerSource(visitedCustomersIndexes, cluster, depotIndex, vertexIndex);

    int selectedSubClusterIndex = -1;
    if(primarySubClusterSelectionProbability != nullptr) {

        // for(int index = 0; index < cluster->primariesCount; index++) {
        //     std::cout << "Primary2\nselectionProbability[" << index << "]: ";
        //     std::cout << primarySubClusterSelectionProbability[index] << "\n";
        // }

        // double probabilitiesSum = sumArray(primarySubClusterSelectionProbability, cluster->primariesCount);
        // std::cout << "probabilitiesSum: " << probabilitiesSum << "\n";

        selectedSubClusterIndex = rouletteWheelSelection(
            primarySubClusterSelectionProbability,
            cluster->primariesCount
        );
    } else { //if there is no customer unvisited for all primary clusters, choose non primary cluster
        selectedSubClusterIndex = selectSubClusterNonPrimaryFromCustomerSource(visitedCustomersIndexes, vertexIndex);
    }

    // if(selectedSubClusterIndex == -1) {
    //     // //std::cout << "selectSubCluster error!\n";
    //     Cluster* cluster = nullptr;
    //     if(vertexIndex == -1) { //depot
    //         cluster = &depotClusters[depotIndex];
    //     } else {
    //         cluster = &customerClusters[vertexIndex];
    //         //std::cout << "CustomerCluster[" << vertexIndex << "]: ";
    //         cluster->print();
    //         //std::cout << "\n";
    //     }
    // }

    free(primarySubClusterSelectionProbability);

    return selectedSubClusterIndex;
}

double* StodolaInspiredAntSystem::getPrimarySubClusterSelectionProbability(double* heuristicInformationAverage, double* pheromoneConcentrationAverage, int primarySubClustersCount) {
    
    double* primarySubClusterSelectionProbability = (double*) malloc(primarySubClustersCount * sizeof(double));
    fillArray(primarySubClusterSelectionProbability, primarySubClustersCount, 0.0);

    for(int subClusterIndex = 0; subClusterIndex < primarySubClustersCount; subClusterIndex++) {
        
        // std::cout << "subCluster[" << subClusterIndex << "]: ";
        // std::cout << heuristicInformationAverage[subClusterIndex] << " - ";
        // std::cout << pheromoneConcentrationAverage[subClusterIndex] << "\n";
        primarySubClusterSelectionProbability[subClusterIndex] = 1;
        primarySubClusterSelectionProbability[subClusterIndex] *= pow(heuristicInformationAverage[subClusterIndex], distanceProbabilityCoef);
        primarySubClusterSelectionProbability[subClusterIndex] *= pow(pheromoneConcentrationAverage[subClusterIndex], pheromoneProbabilityCoef);
    }

    return primarySubClusterSelectionProbability;
}

double* StodolaInspiredAntSystem::getPrimarySubClusterSelectionProbabilityFromCustomerSource(
    int* visitedCustomersIndexes, 
    Cluster* cluster,
    int depotIndex,
    int customerIndex
) {

    double* heuristicInformationAverage = (double*) malloc(cluster->primariesCount * sizeof(double));
    double* pheromoneConcentrationAverage = (double*) malloc(cluster->primariesCount * sizeof(double));
    
    fillArray(pheromoneConcentrationAverage, cluster->primariesCount, 0.0);
    fillArray(heuristicInformationAverage, cluster->primariesCount, 0.0);

    int consideredCustomersCountSum = 0;
    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {

        int consideredCustomersCount = 0;
        double heuristicInformationSum = 0;
        double pheromoneConcentrationSum = 0;

        SubCluster* subCluster = &cluster->subClusters[subClusterIndex];
        for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

            int neighborCustomerIndex = subCluster->elements[memberIndex];
            if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

                heuristicInformationSum += problemInstance.verticesDistanceMatrix[customerIndex][neighborCustomerIndex];
                pheromoneConcentrationSum += pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex];
                consideredCustomersCount++;
            }
        }

        if(consideredCustomersCount > 0) {
            heuristicInformationAverage[subClusterIndex] = consideredCustomersCount * heuristicInformationSum;
            pheromoneConcentrationAverage[subClusterIndex] = pheromoneConcentrationSum / consideredCustomersCount;
            consideredCustomersCountSum += consideredCustomersCount;
        }
    }

    double* primarySubClusterSelectionProbability = nullptr;
    if(consideredCustomersCountSum > 0) {
        primarySubClusterSelectionProbability = getPrimarySubClusterSelectionProbability(heuristicInformationAverage, pheromoneConcentrationAverage, cluster->primariesCount);
    }

    free(heuristicInformationAverage);
    free(pheromoneConcentrationAverage);

    return primarySubClusterSelectionProbability;
}

int StodolaInspiredAntSystem::selectSubClusterNonPrimaryFromCustomerSource(int* visitedCustomersIndexes, int customerIndex) {
    
    Cluster* cluster = &verticesClusters[customerIndex];

    for(int subClusterIndex = cluster->primariesCount; subClusterIndex < cluster->size; subClusterIndex++) {

        SubCluster* subCluster = &cluster->subClusters[subClusterIndex];

        int consideredCustomersCount = 0;
        for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

            int neighborCustomerIndex = subCluster->elements[memberIndex];
            if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {
                consideredCustomersCount++;
            }
        }

        if(consideredCustomersCount > 0) {
            return subClusterIndex;
        }
    }

    return -1;
}

int StodolaInspiredAntSystem::selectCustomer(int* visitedCustomersIndexes, int vertexIndex, int depotIndex, int subClusterIndex) {
    
    double* customerSelectionProbability = nullptr;
    SubCluster* subCluster = &verticesClusters[vertexIndex].subClusters[subClusterIndex];

    customerSelectionProbability = getCustomerSelectionProbabilityFromCustomerSource(visitedCustomersIndexes, subCluster, depotIndex, vertexIndex);

    // if(customerSelectionProbability == nullptr) {
    //     return -1;
    // }

    int memberIndex = rouletteWheelSelection(
        customerSelectionProbability,
        subCluster->size
    );

    // if(memberIndex == -1) {
    //     if(vertexIndex == -1) {
    //         //std::cout << "Depot[" << depotIndex << "]\n";
    //         for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

    //             int customerIndex = subCluster->elements[memberIndex];
    //             if(visitedCustomersIndexes[customerIndex] != 1) {

    //                 //std::cout << pow(problemInstance.depotToCustomerDistanceMatrix[depotIndex][customerIndex], -distanceProbabilityCoef) << "\n";
    //                 //std::cout << pow(depotPheromoneMatrix[depotIndex][customerIndex], pheromoneProbabilityCoef) << "\n";
    //             }
    //         }
    //     } else {
    //         //std::cout << "Customer[" << vertexIndex << "]\n";
    //         for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {
            
    //             int neighborCustomerIndex = subCluster->elements[memberIndex];
    //             if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

    //                 //std::cout << pow(problemInstance.verticesDistanceMatrix[vertexIndex][neighborCustomerIndex], -distanceProbabilityCoef) << "\n";
    //                 //std::cout << pow(pheromoneMatrix[depotIndex][vertexIndex][neighborCustomerIndex], pheromoneProbabilityCoef) << "\n";
    //             }
    //         }
    //     }
    // }

    free(customerSelectionProbability);

    return subCluster->elements[memberIndex];
}

double* StodolaInspiredAntSystem::getCustomerSelectionProbabilityFromCustomerSource(int* visitedCustomersIndexes, SubCluster* subCluster, int depotIndex, int customerIndex) {
  
    double* customerSelectionProbability = (double*) malloc(subCluster->size * sizeof(double));
    fillArray(customerSelectionProbability, subCluster->size, 0.0);

    for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {
        
        int neighborCustomerIndex = subCluster->elements[memberIndex];
        if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

            customerSelectionProbability[memberIndex] += pow(problemInstance.verticesDistanceMatrix[customerIndex][neighborCustomerIndex], -distanceProbabilityCoef);
            customerSelectionProbability[memberIndex] *= pow(pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex], pheromoneProbabilityCoef);
        }
    }

    return customerSelectionProbability;
}

double StodolaInspiredAntSystem::calculateInformationEntropy(int** generationEdgesOcurrenceSum, int generationEdgesSum) {
    
    double informationEntropy = 0;

    for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
        for(int neighborCustomerIndex = 0; neighborCustomerIndex < problemInstance.customersCount; neighborCustomerIndex++) {
            
            double edgeOcurrenceProbability = generationEdgesOcurrenceSum[customerIndex][neighborCustomerIndex];
            if(edgeOcurrenceProbability > 0) {

                edgeOcurrenceProbability /= generationEdgesSum;
                informationEntropy += (edgeOcurrenceProbability * log(edgeOcurrenceProbability));
            }
        }
    }

    informationEntropy *= -1;

    return informationEntropy;
}

int StodolaInspiredAntSystem::updateGenerationEdgesOccurrenceCount(const Solution& solution, int** edgesOccurrenceCount) {

    //TODO: it could be needed to use the depot-customer edges

    int edgesCount = 0;
    // int depotEdges = 0;
    for(int depotIndex = 0; depotIndex < solution.depotsCount; depotIndex++) {
        
        Route* route = &solution.routes[depotIndex];
        for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {

            SubRoute* subRoute = &route->subRoutes[subRouteIndex];
            
            // int firstCustomerIndex = subRoute->first();
            // int lastCustomerIndex = subRoute->last();

            // edgesOccurrenceCount[depotIndex][firstCustomerIndex] += 1;
            // edgesOccurrenceCount[depotIndex][lastCustomerIndex] += 1;
            // depotEdges += 1;

            for(int memberIndex = 0; memberIndex < subRoute->length - 1; memberIndex++) {
                int customerIndex = subRoute->members[memberIndex];
                int neighborCustomerIndex = subRoute->members[memberIndex + 1];

                edgesOccurrenceCount[customerIndex][neighborCustomerIndex] += 1;
                edgesCount += 1;
            }
        }
    }

    // std::cout << "edgesCount: " << edgesCount << "\n";

    return edgesCount;
}

void normalizeValues(double* selectionProbability, int candidatesCount) {
    
    double probabilitiesSum = sumArray(selectionProbability, candidatesCount);

    for(int index = 0; index < candidatesCount; index++) {
        selectionProbability[index] /= probabilitiesSum;
    }
}

int rouletteWheelSelection(double* selectionProbability, int candidatesCount) {
    
    normalizeValues(selectionProbability, candidatesCount);
    
    double randomValue = ((double)rand() / RAND_MAX);
    
    double cumulativeProbability = 0;
    for(int index = 0; index < candidatesCount; index++) {

        cumulativeProbability += selectionProbability[index];
        if (randomValue <= cumulativeProbability) {
            return index;
        }
    }

    return -1;
}