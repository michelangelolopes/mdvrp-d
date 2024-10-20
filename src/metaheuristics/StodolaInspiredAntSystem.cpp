#include "../../include/metaheuristics/StodolaInspiredAntSystem.h"

#include <cmath>    // ceil()
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <iostream>

#include "../../include/utils/ArrayUtils.h"

void StodolaInspiredAntSystem::create(int primarySubClustersMaxCount, int subClusterMaxSize) {
    createClusters(primarySubClustersMaxCount, subClusterMaxSize);
    initializePheromoneMatrices();
}

void StodolaInspiredAntSystem::finalize() {
    
    problemInstance.finalize();

    if(bestSolution != nullptr) {
        bestSolution->finalize();
        delete bestSolution;
    }

    if(pheromoneMatrix != nullptr) {

        for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
            freeMatrix((void**) pheromoneMatrix[depotIndex], problemInstance.customersCount);
        }

        free(pheromoneMatrix);
    }

    frame.finalize();

    if(depotPheromoneMatrix != nullptr) {
        freeMatrix((void**) depotPheromoneMatrix, problemInstance.depotsCount);
    }

    if(customerClusters != nullptr) {

        for(int index = 0; index < problemInstance.customersCount; index++) {
            customerClusters[index].finalize();
        }

        free(customerClusters);
    }

    if(depotClusters != nullptr) {

        for(int index = 0; index < problemInstance.depotsCount; index++) {
            depotClusters[index].finalize();
        }

        free(depotClusters);
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
    createCustomerClusters(primarySubClustersMaxCount, subClusterMaxSize);
    createDepotClusters(primarySubClustersMaxCount, subClusterMaxSize);
}

void StodolaInspiredAntSystem::createCustomerClusters(int primarySubClustersMaxCount, int subClusterMaxSize) {

    customerClusters = (CustomerToCustomerCluster*) calloc(problemInstance.customersCount, sizeof(CustomerToCustomerCluster));

    for(int index = 0; index < problemInstance.customersCount; index++) {
        customerClusters[index] = CustomerToCustomerCluster(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, index);
    }
}

void StodolaInspiredAntSystem::createDepotClusters(int primarySubClustersMaxCount, int subClusterMaxSize) {

    depotClusters = (DepotToCustomerCluster*) calloc(problemInstance.depotsCount, sizeof(DepotToCustomerCluster));

    for(int index = 0; index < problemInstance.depotsCount; index++) {
        depotClusters[index] = DepotToCustomerCluster(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, index);
    }
}

void StodolaInspiredAntSystem::initializePheromoneMatrices() {

    pheromoneMatrix = (double***) malloc(problemInstance.depotsCount * sizeof(double**));

    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
        pheromoneMatrix[depotIndex] = (double**) callocMatrix(problemInstance.customersCount, problemInstance.customersCount, sizeof(double*), sizeof(double));

        for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
            for(int neighborCustomerIndex = 0; neighborCustomerIndex < problemInstance.customersCount; neighborCustomerIndex++) {
                pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex] = 1;
            }
        }
    }

    depotPheromoneMatrix = (double**) calloc(problemInstance.depotsCount, sizeof(double*));

    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
        depotPheromoneMatrix[depotIndex] = (double*) calloc(problemInstance.customersCount, sizeof(double));

        for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
            depotPheromoneMatrix[depotIndex][customerIndex] = 1;
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

void StodolaInspiredAntSystem::reinforcePheromoneMatrix(Solution* consideredSolution) {

    double pheromoneReinforcingValue = pheromoneUpdateCoef * (bestSolution->fitness / consideredSolution->fitness);

    //obs: checking precedence is not needed, because only the pheromone of visitedCustomers from the route will be updated
    for(int depotIndex = 0; depotIndex < consideredSolution->depotsCount; depotIndex++) {
        reinforcePheromoneMatrixInRoute(consideredSolution->routes[depotIndex], depotIndex, pheromoneReinforcingValue);
    }
    
    temperatureUpdateCoef *= temperatureCoolingCoef;
}

void StodolaInspiredAntSystem::reinforcePheromoneMatrixInRoute(const Route& route, int depotIndex, double pheromoneReinforcingValue) {

    for(int subRouteIndex = 0; subRouteIndex < route.size; subRouteIndex++) {
        reinforcePheromoneMatrixInSubRoute(route.subRoutes[subRouteIndex], depotIndex, pheromoneReinforcingValue);
    }
}

void StodolaInspiredAntSystem::reinforcePheromoneMatrixInSubRoute(const SubRoute& subRoute, int depotIndex, double pheromoneReinforcingValue) {

    int firstCustomerIndex = subRoute.members[0];
    depotPheromoneMatrix[depotIndex][firstCustomerIndex] += pheromoneReinforcingValue;

    for(int memberIndex = 0; memberIndex < subRoute.length - 1; memberIndex++) {

        int customerIndex = subRoute.members[memberIndex];
        int neighborCustomerIndex = subRoute.members[memberIndex + 1];
        pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex] += pheromoneReinforcingValue;
    }

    int lastCustomerIndex = subRoute.last();
    depotPheromoneMatrix[depotIndex][lastCustomerIndex] += pheromoneReinforcingValue;
}

void StodolaInspiredAntSystem::evaporatePheromoneMatrix() {

    double pheromoneEvaporatingValue = (1 - pheromoneEvaporationCoef);

    //obs: checking precedence is not needed, because only the pheromone of visitedCustomers from the route will be updated
    for(int depotIndex = 0; depotIndex < bestSolution->depotsCount; depotIndex++) {
        evaporatePheromoneMatrixInRoute(bestSolution->routes[depotIndex], depotIndex, pheromoneEvaporatingValue);
    }
}

void StodolaInspiredAntSystem::evaporatePheromoneMatrixInRoute(const Route& route, int depotIndex, double pheromoneEvaporatingValue) {

    for(int subRouteIndex = 0; subRouteIndex < route.size; subRouteIndex++) {
        evaporatePheromoneMatrixInSubRoute(route.subRoutes[subRouteIndex], depotIndex, pheromoneEvaporatingValue);
    }
}

void StodolaInspiredAntSystem::evaporatePheromoneMatrixInSubRoute(const SubRoute& subRoute, int depotIndex, double pheromoneEvaporatingValue) {

    int firstCustomerIndex = subRoute.members[0];
    depotPheromoneMatrix[depotIndex][firstCustomerIndex] *= pheromoneEvaporatingValue;

    for(int memberIndex = 0; memberIndex < subRoute.length - 1; memberIndex++) {

        int customerIndex = subRoute.members[memberIndex];
        int neighborCustomerIndex = subRoute.members[memberIndex + 1];
        pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex] *= pheromoneEvaporatingValue;
    }

    int lastCustomerIndex = subRoute.last();
    depotPheromoneMatrix[depotIndex][lastCustomerIndex] *= pheromoneEvaporatingValue;
}

void StodolaInspiredAntSystem::updateEvaporationCoef(int** generationEdgesOcurrenceSum, int generationEdgesSum) {

    double informationEntropy = calculateInformationEntropy(generationEdgesOcurrenceSum, generationEdgesSum);
    double informationEntropyMin = -1 * log((double)antsCount / generationEdgesSum);
    double informationEntropyMax = -1 * log(1.00 / generationEdgesSum);

    double relevantInformationEntropy = (informationEntropy - informationEntropyMin) / (informationEntropyMax - informationEntropyMin);

    pheromoneEvaporationCoef = pheromoneEvaporationCoefMin;
    pheromoneEvaporationCoef += (pheromoneEvaporationCoefMax - pheromoneEvaporationCoefMin) * relevantInformationEntropy;
}

void StodolaInspiredAntSystem::run() {

    srand((unsigned int)time(0));

    int maxIter = 10000;
    int iterIndex = 0;
    int generationEdgesCount = 0;
    Solution* generationBestSolution = nullptr;
    int** generationEdgesOccurrenceCount = (int**) callocMatrix(problemInstance.customersCount, sizeof(int*), sizeof(int));
    
    int globalImprovementsCount = 0;
    int intervalImprovementsCount = 0;

    while(iterIndex < maxIter) { //TODO: adjust termination condition

        // std::cout << "--- generation: " << iterIndex << "\n";
        
        fillMatrix(generationEdgesOccurrenceCount, problemInstance.customersCount, 0);
        
        for(int antIndex = 0; antIndex < antsCount; antIndex++) {

            Solution antSolution = buildAntSolution();
            
            // std::cout << "------ ant: " << antIndex << " - ";
            // std::cout << antSolution.fitness << "\n";

            //TODO: it could be needed to use the depot-customer edges
            generationEdgesCount += updateGenerationEdgesOccurrenceCount(antSolution, generationEdgesOccurrenceCount);

            if(generationBestSolution == nullptr) {
                generationBestSolution = new Solution(antSolution);
            } else if(antSolution.fitness < generationBestSolution->fitness) {
                generationBestSolution->finalize();
                delete generationBestSolution;

                generationBestSolution = new Solution(antSolution);
            } else {
                antSolution.finalize();
            }
        }

        //TODO: local optimization
        // if(iterIndex % localOptimizationFrequency == 0) {
        // }

        if(bestSolution == nullptr) {
            // //std::cout << "generation " << iterIndex << " - best solution\n";
            bestSolution = generationBestSolution;
            intervalImprovementsCount += 1;
            // bestSolution->print();

            reinforcePheromoneMatrix(bestSolution);
        } else if(generationBestSolution->fitness < bestSolution->fitness) {
            bestSolution->finalize();
            delete bestSolution;

            // //std::cout << "generation " << iterIndex << " - best solution\n";
            bestSolution = generationBestSolution;
            intervalImprovementsCount += 1;
            // bestSolution->print();

            reinforcePheromoneMatrix(bestSolution);
        } else {
            reinforcePheromoneMatrixWithProbability(generationBestSolution);

            generationBestSolution->finalize();
            delete generationBestSolution;
        }

        updateEvaporationCoef(generationEdgesOccurrenceCount, generationEdgesCount);
        evaporatePheromoneMatrix();

        iterIndex += 1;

        generationBestSolution = nullptr;
        generationEdgesCount = 0;

        if(iterIndex % 1000 == 0 && intervalImprovementsCount > 0) {
            globalImprovementsCount += intervalImprovementsCount;

            std::cout << "generation: " << iterIndex << " - ";
            std::cout << "globalImprovements: " << globalImprovementsCount << " - ";
            std::cout << "intervalImprovements: " << intervalImprovementsCount << "\n";
            
            bestSolution->print();
            
            intervalImprovementsCount = 0;
        }
    }

    freeMatrix((void**) generationEdgesOccurrenceCount, problemInstance.customersCount);
}

Solution StodolaInspiredAntSystem::buildAntSolution() {

    // int* currentVertexIndex = (int*) calloc(problemInstance.depotsCount, sizeof(int));
    // int* currentTruckLoad = (int*) calloc(problemInstance.depotsCount, sizeof(int));

    // for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
    //     currentVertexIndex[depotIndex] = -1; //when it is a depot
    //     // currentTruckLoad[depotIndex] = 0;
    // }

    int unvisitedCustomersCount = problemInstance.customersCount;
    int* visitedCustomersIndexes = (int*) calloc(problemInstance.customersCount, sizeof(int));

    Solution antSolution(problemInstance.depotsCount, MinimizationType::MAX_TIME_SPENT, problemInstance.customersCount);

    while(unvisitedCustomersCount > 0) {

        // std::cout << "--------- unvisitedCustomersCount: " << unvisitedCustomersCount << "\n";
        // std::cout << "--------- unvisitedCustomers: ";
        // for(int i = 0; i < problemInstance.customersCount; i++) {
        //     if(visitedCustomersIndexes[i] != 1) {
        //         std::cout << i << " ";
        //     }
        // }
        // std::cout << "\n";
        int depotIndex = selectDepot(visitedCustomersIndexes, antSolution.routes);
        // std::cout << "------------ depotIndex: " << depotIndex << "\n";
        // std::cout << "------------ currentVertexIndex: " << antSolution.routes[depotIndex].last() << "\n";
        // if(antSolution.routes[depotIndex].last() != -1) {
        //     std::cout << "------------ currentCluster: ";
        //     customerClusters[antSolution.routes[depotIndex].last()].print(visitedCustomersIndexes);
        //     std::cout << "\n";

        //     std::cout << "------------ fullCluster: ";
        //     customerClusters[antSolution.routes[depotIndex].last()].print();
        //     std::cout << "\n";
        // }
        // std::cout << "------------ currentRoute: ";
        // antSolution.routes[depotIndex].print();
        // std::cout << "\n";
        Route* currentRoute = &antSolution.routes[depotIndex];
        int subClusterIndex = selectSubCluster(visitedCustomersIndexes, currentRoute->last(), depotIndex);
        // std::cout << "------------ subClusterIndex: " << subClusterIndex << "\n";
        int customerIndex = selectCustomer(visitedCustomersIndexes, currentRoute->last(), depotIndex, subClusterIndex);
        // std::cout << "------------ customerIndex: " << customerIndex << "\n";

        if(antSolution.routes[depotIndex].getCurrentLoad() + problemInstance.customers[customerIndex].demand > problemInstance.depots[depotIndex].truck.capacity) {
            antSolution.routes[depotIndex].expand();
        }

        antSolution.routes[depotIndex].insert(customerIndex);
        antSolution.routes[depotIndex].updateCurrentLoad(problemInstance.customers[customerIndex].demand);
        // antSolution.print();

        // currentVertexIndex[depotIndex] = customerIndex;
        // currentTruckLoad[depotIndex] += problemInstance.customers[customerIndex].demand;

        visitedCustomersIndexes[customerIndex] = 1;
        unvisitedCustomersCount--;
    }

    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
        antSolution.routes[depotIndex].shrink();
    }

    // antSolution.print();
    antSolution.updateFitness(problemInstance);

    // free(currentVertexIndex);
    // free(currentTruckLoad);
    free(visitedCustomersIndexes);

    return antSolution;
}

int StodolaInspiredAntSystem::selectDepot(int* visitedCustomersIndexes, Route* routes) {

    double* depotSelectionProbability = (double*) calloc(problemInstance.depotsCount, sizeof(double));

    int consideredCustomersCount = 0;
    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {

        int vertexIndex = routes[depotIndex].last();
        if(vertexIndex == -1) { //depot
            consideredCustomersCount += updateDepotSelectionProbabilityFromDepotSource(visitedCustomersIndexes, depotSelectionProbability, depotIndex);
        } else { //customer
            consideredCustomersCount += updateDepotSelectionProbabilityFromCustomerSource(visitedCustomersIndexes, depotSelectionProbability, depotIndex, vertexIndex);
        }
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

int StodolaInspiredAntSystem::updateDepotSelectionProbabilityFromDepotSource(int* visitedCustomersIndexes, double* depotSelectionProbability, int depotIndex) {
    
    Cluster* cluster = &depotClusters[depotIndex];

    int consideredCustomersCount = 0;
    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {

        SubCluster* subCluster = &cluster->subClusters[subClusterIndex];
        for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

            int customerIndex = subCluster->elements[memberIndex];
            if(visitedCustomersIndexes[customerIndex] != 1) {

                consideredCustomersCount++;
                depotSelectionProbability[depotIndex] += depotPheromoneMatrix[depotIndex][customerIndex];
            }
        }
    }

    return consideredCustomersCount;
}

int StodolaInspiredAntSystem::updateDepotSelectionProbabilityFromCustomerSource(int* visitedCustomersIndexes, double* depotSelectionProbability, int depotIndex, int customerIndex) {
    
    Cluster* cluster = &customerClusters[customerIndex];
    
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
    Cluster* cluster = nullptr;

    if(vertexIndex == -1) { //depot
        cluster = &depotClusters[depotIndex];
        primarySubClusterSelectionProbability = getPrimarySubClusterSelectionProbabilityFromDepotSource(visitedCustomersIndexes, cluster, depotIndex);
    } else { //customer
        cluster = &customerClusters[vertexIndex];
        // std::cout << "\nPrimary1\n";
        primarySubClusterSelectionProbability = getPrimarySubClusterSelectionProbabilityFromCustomerSource(visitedCustomersIndexes, cluster, depotIndex, vertexIndex);
    }

    int selectedSubClusterIndex = -1;
    if(primarySubClusterSelectionProbability != nullptr) {

        // for(int index = 0; index < cluster->primariesCount; index++) {
        //     std::cout << "Primary2\nselectionProbability[" << index << "]: ";
        //     std::cout << primarySubClusterSelectionProbability[index] << "\n";
        // }

        selectedSubClusterIndex = rouletteWheelSelection(
            primarySubClusterSelectionProbability,
            cluster->primariesCount
        );
    } else { //if there is no customer unvisited for all primary clusters, choose non primary cluster

        if(vertexIndex == -1) { //depot
            selectedSubClusterIndex = selectSubClusterNonPrimaryFromDepotSource(visitedCustomersIndexes, depotIndex);
        } else {
            selectedSubClusterIndex = selectSubClusterNonPrimaryFromCustomerSource(visitedCustomersIndexes, vertexIndex);
        }
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
    
    double* primarySubClusterSelectionProbability = (double*) calloc(primarySubClustersCount, sizeof(double));
    fillArray(primarySubClusterSelectionProbability, primarySubClustersCount, 0);

    for(int subClusterIndex = 0; subClusterIndex < primarySubClustersCount; subClusterIndex++) {
        
        // std::cout << "subCluster[" << subClusterIndex << "]: ";
        // std::cout << heuristicInformationAverage[subClusterIndex] << "]";
        // std::cout << pheromoneConcentrationAverage[subClusterIndex] << "]\n";
        primarySubClusterSelectionProbability[subClusterIndex] = 1;
        primarySubClusterSelectionProbability[subClusterIndex] *= pow(heuristicInformationAverage[subClusterIndex], distanceProbabilityCoef);
        primarySubClusterSelectionProbability[subClusterIndex] *= pow(pheromoneConcentrationAverage[subClusterIndex], pheromoneProbabilityCoef);
    }

    return primarySubClusterSelectionProbability;
}

double* StodolaInspiredAntSystem::getPrimarySubClusterSelectionProbabilityFromDepotSource(
    int* visitedCustomersIndexes,
    Cluster* cluster,
    int depotIndex
) {
    
    double* heuristicInformationAverage = (double*) calloc(cluster->primariesCount, sizeof(double));
    double* pheromoneConcentrationAverage = (double*) calloc(cluster->primariesCount, sizeof(double));
    
    fillArray(pheromoneConcentrationAverage, cluster->primariesCount, 0);
    fillArray(heuristicInformationAverage, cluster->primariesCount, 0);

    int consideredCustomersCountSum = 0;
    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {

        int consideredCustomersCount = 0;
        double heuristicInformationSum = 0;
        double pheromoneConcentrationSum = 0;

        SubCluster* subCluster = &cluster->subClusters[subClusterIndex];
        for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

            int customerIndex = subCluster->elements[memberIndex];
            if(visitedCustomersIndexes[customerIndex] != 1) {

                heuristicInformationSum += problemInstance.depotToCustomerDistanceMatrix[depotIndex][customerIndex];
                pheromoneConcentrationSum += depotPheromoneMatrix[depotIndex][customerIndex];
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

double* StodolaInspiredAntSystem::getPrimarySubClusterSelectionProbabilityFromCustomerSource(
    int* visitedCustomersIndexes, 
    Cluster* cluster,
    int depotIndex,
    int customerIndex
) {

    double* heuristicInformationAverage = (double*) calloc(cluster->primariesCount, sizeof(double));
    double* pheromoneConcentrationAverage = (double*) calloc(cluster->primariesCount, sizeof(double));
    
    fillArray(pheromoneConcentrationAverage, cluster->primariesCount, 0);
    fillArray(heuristicInformationAverage, cluster->primariesCount, 0);

    int consideredCustomersCountSum = 0;
    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {

        int consideredCustomersCount = 0;
        double heuristicInformationSum = 0;
        double pheromoneConcentrationSum = 0;

        SubCluster* subCluster = &cluster->subClusters[subClusterIndex];
        for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

            int neighborCustomerIndex = subCluster->elements[memberIndex];
            if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

                heuristicInformationSum += problemInstance.customerToCustomerDistanceMatrix[customerIndex][neighborCustomerIndex];
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

int StodolaInspiredAntSystem::selectSubClusterNonPrimaryFromDepotSource(int* visitedCustomersIndexes, int depotIndex) {
    
    Cluster* cluster = &depotClusters[depotIndex];

    for(int subClusterIndex = cluster->primariesCount; subClusterIndex < cluster->size; subClusterIndex++) {

        SubCluster* subCluster = &cluster->subClusters[subClusterIndex];

        int consideredCustomersCount = 0;
        for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

            int customerIndex = subCluster->elements[memberIndex];
            if(visitedCustomersIndexes[customerIndex] != 1) {
                consideredCustomersCount++;
            }
        }

        if(consideredCustomersCount > 0) {
            return subClusterIndex;
        }
    }

    return -1;
}

int StodolaInspiredAntSystem::selectSubClusterNonPrimaryFromCustomerSource(int* visitedCustomersIndexes, int customerIndex) {
    
    Cluster* cluster = &customerClusters[customerIndex];

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
    SubCluster* subCluster = nullptr;

    if(vertexIndex == -1) { //depot
        subCluster = &depotClusters[depotIndex].subClusters[subClusterIndex];

        // //std::cout << "cluster->size: " << cluster->size << "\n";
        // //std::cout << "depotIndex: " << depotIndex << "\n";
        // //std::cout << "subClusterIndex: " << subClusterIndex << "\n";

        // cluster->print(visitedCustomersIndexes);

        customerSelectionProbability = getCustomerSelectionProbabilityFromDepotSource(visitedCustomersIndexes, subCluster, depotIndex);
    } else { //customer
        subCluster = &customerClusters[vertexIndex].subClusters[subClusterIndex];

        // std::cout << "cluster->size: " << customerClusters[vertexIndex].size << "\n";
        // std::cout << "customerIndex: " << vertexIndex << "\n";
        // std::cout << "subClusterIndex: " << subClusterIndex << "\n";

        // customerClusters[vertexIndex].print(visitedCustomersIndexes);
        // std::cout << "\n";

        customerSelectionProbability = getCustomerSelectionProbabilityFromCustomerSource(visitedCustomersIndexes, subCluster, depotIndex, vertexIndex);
    }

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

    //                 //std::cout << pow(problemInstance.customerToCustomerDistanceMatrix[vertexIndex][neighborCustomerIndex], -distanceProbabilityCoef) << "\n";
    //                 //std::cout << pow(pheromoneMatrix[depotIndex][vertexIndex][neighborCustomerIndex], pheromoneProbabilityCoef) << "\n";
    //             }
    //         }
    //     }
    // }

    free(customerSelectionProbability);

    return subCluster->elements[memberIndex];
}

double* StodolaInspiredAntSystem::getCustomerSelectionProbabilityFromDepotSource(int* visitedCustomersIndexes, SubCluster* subCluster, int depotIndex) {

    double* customerSelectionProbability = (double*) calloc(subCluster->size, sizeof(double));
    fillArray(customerSelectionProbability, subCluster->size, 0);

    for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

        int customerIndex = subCluster->elements[memberIndex];
        if(visitedCustomersIndexes[customerIndex] != 1) {

            customerSelectionProbability[memberIndex] += pow(problemInstance.depotToCustomerDistanceMatrix[depotIndex][customerIndex], -distanceProbabilityCoef);
            customerSelectionProbability[memberIndex] *= pow(depotPheromoneMatrix[depotIndex][customerIndex], pheromoneProbabilityCoef);
        }
    }

    return customerSelectionProbability;
}

double* StodolaInspiredAntSystem::getCustomerSelectionProbabilityFromCustomerSource(int* visitedCustomersIndexes, SubCluster* subCluster, int depotIndex, int customerIndex) {
  
    double* customerSelectionProbability = (double*) calloc(subCluster->size, sizeof(double));
    fillArray(customerSelectionProbability, subCluster->size, 0);

    for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {
        
        int neighborCustomerIndex = subCluster->elements[memberIndex];
        if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

            customerSelectionProbability[memberIndex] += pow(problemInstance.customerToCustomerDistanceMatrix[customerIndex][neighborCustomerIndex], -distanceProbabilityCoef);
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

    int edgesCount = 0;
    for(int depotIndex = 0; depotIndex < solution.depotsCount; depotIndex++) {
        
        Route* route = &solution.routes[depotIndex];
        for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {

            SubRoute* subRoute = &route->subRoutes[subRouteIndex];
            for(int memberIndex = 0; memberIndex < subRoute->length - 1; memberIndex++) {
                int customerIndex = subRoute->members[memberIndex];
                int neighborCustomerIndex = subRoute->members[memberIndex + 1];

                edgesOccurrenceCount[customerIndex][neighborCustomerIndex] += 1;
                edgesCount += 1;
            }
        }
    }

    return edgesCount;
}

void normalizeValues(double* selectionProbability, int candidatesCount) {
    
    double cumulativeProbability = 0;
    for(int index = 0; index < candidatesCount; index++) {
        cumulativeProbability += selectionProbability[index];
    }

    for(int index = 0; index < candidatesCount; index++) {
        selectionProbability[index] /= cumulativeProbability;
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