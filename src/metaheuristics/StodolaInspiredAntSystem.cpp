#include "../../include/metaheuristics/StodolaInspiredAntSystem.hpp"

#include <chrono>
#include <cmath>    // ceil()
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <functional>
#include <iostream>

#include "../../include/utils/ArrayUtils.hpp"
#include "../../include/utils/MathUtils.hpp"

void StodolaInspiredAntSystem::create(int primarySubClustersMaxCount, int subClusterMaxSize) {

    if(distanceProbabilityCoef == 1 && pheromoneProbabilityCoef == 1) {
        weightedValue = [](double value, double weight) { return value; };
    } else {
        weightedValue = [](double value, double weight) { return pow(value, weight); };
    }

    createClusters(primarySubClustersMaxCount, subClusterMaxSize);
    initializePheromoneMatrices();
}

void StodolaInspiredAntSystem::finalize() {
    
    problemInstance.finalize();
    bestSolution.finalize();

    if(pheromoneMatrix != nullptr) {

        for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
            freeMatrix(pheromoneMatrix[depotIndex], problemInstance.verticesCount);
        }

        free(pheromoneMatrix);
    }

    frame.finalize();

    if(verticesClusters != nullptr) {

        for(int vertexIndex = 0; vertexIndex < problemInstance.verticesCount; vertexIndex++) {
            verticesClusters[vertexIndex].finalize();
        }

        free(verticesClusters);
    }
}

void StodolaInspiredAntSystem::print() {
    //TODO: check what is needed
}

void StodolaInspiredAntSystem::createClusters(int primarySubClustersMaxCount, int subClusterMaxSize) {

    verticesClusters = (Cluster*) malloc(problemInstance.verticesCount * sizeof(Cluster));

    for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
        verticesClusters[customerIndex] = Cluster(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, customerIndex);
    }

    for(int depotVertexIndex = problemInstance.getDepotVertexIndex(0); depotVertexIndex < problemInstance.verticesCount; depotVertexIndex++) {
        verticesClusters[depotVertexIndex] = Cluster(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, depotVertexIndex);
    }
}

void StodolaInspiredAntSystem::initializePheromoneMatrices() {

    pheromoneMatrix = (double***) malloc(problemInstance.depotsCount * sizeof(double**));

    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
        pheromoneMatrix[depotIndex] = (double**) mallocMatrix(problemInstance.verticesCount, problemInstance.customersCount, sizeof(double*), sizeof(double));
        fillMatrix(pheromoneMatrix[depotIndex], problemInstance.verticesCount, problemInstance.customersCount, 1.0);
    }
}

void StodolaInspiredAntSystem::updatePheromoneMatrix(const Solution& consideredSolution, double updateValue, double (*operationFunction) (double, double)) {

    //obs: checking precedence is not needed, because only the pheromone of visitedCustomers from the route will be updated
    for(int depotIndex = 0; depotIndex < consideredSolution.depotsCount; depotIndex++) {
        
        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);
        Route* route = &consideredSolution.routes[depotIndex];

        for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {
        
            SubRoute* subRoute = &route->subRoutes[subRouteIndex];

            int firstCustomerIndex = subRoute->first();
            int lastCustomerIndex = subRoute->last();

            pheromoneMatrix[depotIndex][depotVertexIndex][firstCustomerIndex] = operationFunction(
                pheromoneMatrix[depotIndex][depotVertexIndex][firstCustomerIndex],
                updateValue
            );
            pheromoneMatrix[depotIndex][depotVertexIndex][lastCustomerIndex] = operationFunction(
                pheromoneMatrix[depotIndex][depotVertexIndex][lastCustomerIndex],
                updateValue
            );

            for(int memberIndex = 0; memberIndex < subRoute->length - 1; memberIndex++) {

                int customerIndex = subRoute->members[memberIndex];
                int neighborCustomerIndex = subRoute->members[memberIndex + 1];
                pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex] = operationFunction(
                    pheromoneMatrix[depotIndex][customerIndex][neighborCustomerIndex],
                    updateValue
                );
            }
        }
    }
}

void StodolaInspiredAntSystem::reinforcePheromoneMatrixWithProbability(const Solution& generationBestSolution) {

    double bestSolutionProbability = generationBestSolution.fitness - bestSolution.fitness;
    bestSolutionProbability /= bestSolution.fitness;
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

void StodolaInspiredAntSystem::reinforcePheromoneMatrix(const Solution& consideredSolution) {

    double pheromoneReinforcingValue = pheromoneUpdateCoef * (bestSolution.fitness / consideredSolution.fitness);

    updatePheromoneMatrix(consideredSolution, pheromoneReinforcingValue, add);
    
    temperatureUpdateCoef *= temperatureCoolingCoef;
}

void StodolaInspiredAntSystem::evaporatePheromoneMatrix() {

    double pheromoneEvaporatingValue = (1 - pheromoneEvaporationCoef);

    updatePheromoneMatrix(bestSolution, pheromoneEvaporatingValue, multiply);
}

void StodolaInspiredAntSystem::updateEvaporationCoef(double informationEntropy, double informationEntropyMin, double informationEntropyMax) {

    double relevantInformationEntropyCoef = (informationEntropy - informationEntropyMin) / (informationEntropyMax - informationEntropyMin);

    pheromoneEvaporationCoef = pheromoneEvaporationCoefMin;
    pheromoneEvaporationCoef += (pheromoneEvaporationCoefMax - pheromoneEvaporationCoefMin) * relevantInformationEntropyCoef;
}

int StodolaInspiredAntSystem::hasAchievedTerminationCondition(int iterationsCount, int iterationsWithoutImprovementCount, double currentOptimizationTime, double informationEntropyCoef) {

    return (iterationsCount >= maxIterations) ||
        (iterationsWithoutImprovementCount >= maxIterationsWithoutImprovement) ||
        (currentOptimizationTime >= maxOptimizationTime) ||
        (informationEntropyCoef < minInformationEntropyCoef);
}

void StodolaInspiredAntSystem::run() {

    std::chrono::time_point startOptimizationTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point endOptimizationTime = startOptimizationTime;
    std::chrono::duration<double> currentOptimizationTime = endOptimizationTime - startOptimizationTime;
    
    int primarySubClustersCout = verticesClusters[0].primariesCount;
    int generationEdgesCount = 0;
    int globalImprovementsCount = 0;
    int iterationsCount = 0;
    int iterationsWithoutImprovementCount = 0;
    double informationEntropy = -1;
    double informationEntropyMin = -1;
    double informationEntropyMax = -1;
    double informationEntropyCoef = 100;

    int* visitedCustomersIndexes = (int*) malloc(problemInstance.customersCount * sizeof(int));
    double* selectionProbability = (double*) malloc(problemInstance.customersCount * sizeof(double));
    double* heuristicInformationAverage = (double*) malloc(primarySubClustersCout * sizeof(double));
    double* pheromoneConcentrationAverage = (double*) malloc(primarySubClustersCout * sizeof(double));

    int** generationEdgesOccurrenceCount = (int**) mallocMatrix(problemInstance.verticesCount, problemInstance.verticesCount, sizeof(int*), sizeof(int));

    Solution antSolution(
        problemInstance.depotsCount,
        problemInstance.minimizationType,
        problemInstance.customersCount
    );

    Solution generationBestSolution(
        problemInstance.depotsCount,
        problemInstance.minimizationType,
        problemInstance.customersCount
    );

    //initial solution
    bestSolution = Solution(
        problemInstance.depotsCount,
        problemInstance.minimizationType,
        problemInstance.customersCount
    );

    buildAntRoutes(bestSolution, visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage);

    while(!hasAchievedTerminationCondition(
        iterationsCount, 
        iterationsWithoutImprovementCount,
        currentOptimizationTime.count(),
        informationEntropyCoef
    )) 
    {

        // std::cout << "--- generation: " << iterationsCount << "\n";
        fillMatrix(generationEdgesOccurrenceCount, problemInstance.verticesCount, problemInstance.verticesCount, 0);

        //first ant
        // std::cout << "------ ant: " << 0 << "\n";
        buildAntRoutes(generationBestSolution, visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage);
        generationEdgesCount += updateGenerationEdgesOccurrenceCount(generationBestSolution, generationEdgesOccurrenceCount);
        
        //others ants
        for(int antIndex = 1; antIndex < antsCount; antIndex++) {

            // std::cout << "------ ant: " << antIndex << "\n";

            buildAntRoutes(antSolution, visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage);
            generationEdgesCount += updateGenerationEdgesOccurrenceCount(antSolution, generationEdgesOccurrenceCount);

            if(antSolution.fitness < generationBestSolution.fitness) {
                swap(generationBestSolution, antSolution);
            }
        }

        //TODO: local optimization inter-route exchange
        if(iterationsCount % localOptimizationFrequency == 0) {
            localOptimization(generationBestSolution);
        }

        if(generationBestSolution.fitness < bestSolution.fitness) {

            swap(bestSolution, generationBestSolution);
            reinforcePheromoneMatrix(bestSolution);

            globalImprovementsCount += 1;

            std::cout << "--------------------------------------------------\n";
            std::cout << "globalImproves: " << globalImprovementsCount << " - ";
            std::cout << "fitness: " << bestSolution.fitness << " - ";
            std::cout << "generations: " << iterationsCount << " - ";
            std::cout << "generationsUntilImprove: " << iterationsWithoutImprovementCount  << " - ";

            iterationsWithoutImprovementCount = 0;
        } else {
            reinforcePheromoneMatrixWithProbability(generationBestSolution);
        }

        informationEntropy = calculateInformationEntropy(generationEdgesOccurrenceCount, generationEdgesCount);
        informationEntropyMin = -1 * log2((double)antsCount / generationEdgesCount);
        informationEntropyMax = -1 * log2(1.00 / generationEdgesCount);
        informationEntropyCoef = (informationEntropy - informationEntropyMin) / informationEntropyMin;

        updateEvaporationCoef(informationEntropy, informationEntropyMin, informationEntropyMax);
        evaporatePheromoneMatrix();

        endOptimizationTime = std::chrono::high_resolution_clock::now();
        currentOptimizationTime = endOptimizationTime - startOptimizationTime;

        if(iterationsWithoutImprovementCount == 0) {

            std::cout << "timer: " << currentOptimizationTime.count() << "\n";
            std::cout << "informationEntropy: coef: " << informationEntropyCoef << " - ";
            std::cout << "min: " << informationEntropyMin << " - ";
            std::cout << "cur: " << informationEntropy << " - ";
            std::cout << "max: " << informationEntropyMax << "\n";
        }

        iterationsCount += 1;
        iterationsWithoutImprovementCount += 1;
        generationEdgesCount = 0;
    }

    std::cout << "--------------------------------------------------\n";
    std::cout << "generations: " << iterationsCount << " - ";
    std::cout << "generationsWithoutImprove: " << iterationsWithoutImprovementCount  << " - ";
    std::cout << "timer: " << currentOptimizationTime.count() << "\n";
    
    bestSolution.print();

    antSolution.finalize();
    generationBestSolution.finalize();
    
    free(visitedCustomersIndexes);
    free(selectionProbability);
    free(heuristicInformationAverage);
    free(pheromoneConcentrationAverage);
    freeMatrix(generationEdgesOccurrenceCount, problemInstance.verticesCount);
}

void StodolaInspiredAntSystem::buildAntRoutes(Solution& antSolution, int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage) {

    antSolution.reset();

    int unvisitedCustomersCount = problemInstance.customersCount;
    fillArray(visitedCustomersIndexes, unvisitedCustomersCount, 0);

    while(unvisitedCustomersCount > 0) {

        // std::cout << "--------- unvisitedCustomersCount: " << unvisitedCustomersCount << "\n";
        // std::cout << "--------- unvisitedCustomers: ";
        // printIndexesArray(visitedCustomersIndexes, problemInstance.customersCount, 0);

        int depotIndex = selectDepot(visitedCustomersIndexes, selectionProbability, antSolution.routes);
        // std::cout << "------------ depotIndex: " << depotIndex << "\n";

        Route* currentRoute = &antSolution.routes[depotIndex];

        // std::cout << "------------ currentRoute: ";
        // currentRoute->print();
        // std::cout << "\n";

        int currentVertexIndex = currentRoute->last();
        // std::cout << "------------ currentVertexIndex: " << currentVertexIndex << "\n";
        // std::cout << "------------ currentCluster: ";
        // verticesClusters[currentVertexIndex].print(visitedCustomersIndexes);

        int subClusterIndex = selectSubCluster(visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, depotIndex, currentVertexIndex, pheromoneMatrix);
        // std::cout << "------------ subClusterIndex: " << subClusterIndex << "\n";

        int customerIndex = selectCustomer(visitedCustomersIndexes, selectionProbability, depotIndex, currentVertexIndex, subClusterIndex);
        // std::cout << "------------ customerIndex: " << customerIndex << "\n";

        Customer* nextCustomer = &problemInstance.customers[customerIndex];
        Truck* currentTruck = &problemInstance.depots[depotIndex].truck;

        double updatedTruckLoad = ( currentRoute->currentLoad() + nextCustomer->demand );
        bool willTruckExceedCapacity = updatedTruckLoad > currentTruck->capacity;

        if(willTruckExceedCapacity) {
            currentRoute->expand();
        }

        currentRoute->insert(customerIndex);
        currentRoute->incrementCurrentLoad(nextCustomer->demand);

        visitedCustomersIndexes[customerIndex] = 1;
        unvisitedCustomersCount--;
    }

    antSolution.updateFitness(problemInstance);
}

int StodolaInspiredAntSystem::selectDepot(int* visitedCustomersIndexes, double* selectionProbability, Route* routes) {

    int selectedDepotIndex = -1;

    int hasUnvisitedCustomers = updateDepotSelectionProbability(visitedCustomersIndexes, selectionProbability, routes);
    if(hasUnvisitedCustomers) {
        selectedDepotIndex = rouletteWheelSelection(
            selectionProbability,
            problemInstance.depotsCount
        );
    } else { //TODO: review this temporary logic to consider non primary clusters
        selectedDepotIndex = rand() % problemInstance.depotsCount;
    }

    return selectedDepotIndex;
}

int StodolaInspiredAntSystem::updateDepotSelectionProbability(int* visitedCustomersIndexes, double* depotSelectionProbability, Route* routes) {

    fillArray(depotSelectionProbability, problemInstance.depotsCount, 0.0);

    int consideredCustomersCount = 0;
    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {

        int vertexIndex = routes[depotIndex].last();
        Cluster* cluster = &verticesClusters[vertexIndex];

        for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {

            SubCluster* subCluster = &cluster->subClusters[subClusterIndex];
            for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

                int neighborCustomerIndex = subCluster->elements[memberIndex];
                if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

                    consideredCustomersCount++;
                    depotSelectionProbability[depotIndex] += pheromoneMatrix[depotIndex][vertexIndex][neighborCustomerIndex];
                }
            }
        }
    }
    
    return consideredCustomersCount;
}

int StodolaInspiredAntSystem::selectSubCluster(int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage, int depotIndex, int vertexIndex, double*** pheromoneMatrix) {
    
    int selectedSubClusterIndex = -1;

    int hasUnvisitedCustomers = updatePrimarySubClusterSelectionProbability(visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, depotIndex, vertexIndex, pheromoneMatrix);
    if(hasUnvisitedCustomers) {

        Cluster* cluster = &verticesClusters[vertexIndex];
        selectedSubClusterIndex = rouletteWheelSelection(
            selectionProbability,
            cluster->primariesCount
        );
    } else { //if there is no customer unvisited for all primary clusters, choose non primary cluster
        selectedSubClusterIndex = selectSubClusterNonPrimary(visitedCustomersIndexes, vertexIndex);
    }

    return selectedSubClusterIndex;
}

int StodolaInspiredAntSystem::updatePrimarySubClusterSelectionProbability(
    int* visitedCustomersIndexes, 
    double* primarySubClusterSelectionProbability,
    double* heuristicInformationAverage, 
    double* pheromoneConcentrationAverage,
    int depotIndex,
    int vertexIndex,
    double*** pheromoneMatrix
) {

    Cluster* cluster = &verticesClusters[vertexIndex];
    
    fillArray(pheromoneConcentrationAverage, cluster->primariesCount, 0.0);
    fillArray(heuristicInformationAverage, cluster->primariesCount, 0.0);
    fillArray(primarySubClusterSelectionProbability, cluster->primariesCount, 0.0);

    int consideredCustomersCountSum = 0;
    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {

        int consideredCustomersCount = 0;
        double heuristicInformationSum = 0;
        double pheromoneConcentrationSum = 0;

        SubCluster* subCluster = &cluster->subClusters[subClusterIndex];
        for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

            int neighborCustomerIndex = subCluster->elements[memberIndex];
            if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

                heuristicInformationSum += problemInstance.verticesDistanceMatrix[vertexIndex][neighborCustomerIndex];
                pheromoneConcentrationSum += pheromoneMatrix[depotIndex][vertexIndex][neighborCustomerIndex];
                consideredCustomersCount++;
            }
        }

        if(consideredCustomersCount > 0) {
            heuristicInformationAverage[subClusterIndex] = consideredCustomersCount * heuristicInformationSum;
            pheromoneConcentrationAverage[subClusterIndex] = pheromoneConcentrationSum / consideredCustomersCount;
            consideredCustomersCountSum += consideredCustomersCount;
        }
    }

    if(consideredCustomersCountSum > 0) {
        calculatePrimarySubClusterSelectionProbability(primarySubClusterSelectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, cluster);
    }

    return consideredCustomersCountSum;
}

void StodolaInspiredAntSystem::calculatePrimarySubClusterSelectionProbability( 
    double* primarySubClusterSelectionProbability,
    double* heuristicInformationAverage, 
    double* pheromoneConcentrationAverage,
    Cluster* cluster
) {

    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {
        
        double weightedHeuristicInformationAverage = weightedValue(
            heuristicInformationAverage[subClusterIndex], 
            distanceProbabilityCoef
        );

        double weightedPheromoneConcentrationAverage = weightedValue(
            pheromoneConcentrationAverage[subClusterIndex], 
            pheromoneProbabilityCoef
        );

        primarySubClusterSelectionProbability[subClusterIndex] = weightedHeuristicInformationAverage;
        primarySubClusterSelectionProbability[subClusterIndex] *= weightedPheromoneConcentrationAverage;
    }
}

int StodolaInspiredAntSystem::selectSubClusterNonPrimary(int* visitedCustomersIndexes, int vertexIndex) {
    
    Cluster* cluster = &verticesClusters[vertexIndex];

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

int StodolaInspiredAntSystem::selectCustomer(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int vertexIndex, int subClusterIndex) {

    SubCluster* subCluster = &verticesClusters[vertexIndex].subClusters[subClusterIndex];

    updateCustomerSelectionProbability(visitedCustomersIndexes, selectionProbability, depotIndex, vertexIndex, *subCluster);

    int memberIndex = rouletteWheelSelection(
        selectionProbability,
        subCluster->size
    );

    return subCluster->elements[memberIndex];
}

void StodolaInspiredAntSystem::updateCustomerSelectionProbability(int* visitedCustomersIndexes, double* customerSelectionProbability, int depotIndex, int vertexIndex, const SubCluster& subCluster) {

    fillArray(customerSelectionProbability, subCluster.size, 0.0);

    for(int memberIndex = 0; memberIndex < subCluster.size; memberIndex++) {
        
        int neighborCustomerIndex = subCluster.elements[memberIndex];
        if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

            double weightedPheromoneConcentration = weightedValue(
                pheromoneMatrix[depotIndex][vertexIndex][neighborCustomerIndex], 
                pheromoneProbabilityCoef
            );

            double weightedDistance = weightedValue(
                problemInstance.verticesDistanceMatrix[vertexIndex][neighborCustomerIndex], 
                distanceProbabilityCoef
            );

            customerSelectionProbability[memberIndex] = weightedPheromoneConcentration;
            customerSelectionProbability[memberIndex] /= weightedDistance;
        }
    }
}

double StodolaInspiredAntSystem::calculateInformationEntropy(int** edgesOccurrenceCount, int generationEdgesCount) {
    
    double informationEntropy = 0;

    for(int vertexIndex = 0; vertexIndex < problemInstance.verticesCount; vertexIndex++) {
        for(int neighborVertexIndex = 0; neighborVertexIndex < problemInstance.verticesCount; neighborVertexIndex++) {

            double edgeOcurrenceCount = edgesOccurrenceCount[vertexIndex][neighborVertexIndex];

            if(edgeOcurrenceCount > 0) {

                // std::cout << "(" << vertexIndex << ", " << neighborVertexIndex << "): ";
                // std::cout << edgeOcurrenceCount << " - ";
                // std::cout << generationEdgesCount << " - ";

                double edgeOcurrenceProbability = edgeOcurrenceCount / generationEdgesCount;
                // std::cout << edgeOcurrenceProbability << " - ";

                double balancedEdgeOcurrenceProbability = (edgeOcurrenceProbability * log2(edgeOcurrenceProbability));
                // std::cout << balancedEdgeOcurrenceProbability << "\n";

                informationEntropy += balancedEdgeOcurrenceProbability;
            }
        }
    }
    
    informationEntropy *= -1;

    return informationEntropy;
}

int StodolaInspiredAntSystem::updateGenerationEdgesOccurrenceCount(const Solution& solution, int** edgesOccurrenceCount) {

    //TODO: it could be needed to use the depot-customer edges

    int edgesCount = 0;
    for(int depotIndex = 0; depotIndex < solution.depotsCount; depotIndex++) {
        
        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);
        Route* route = &solution.routes[depotIndex];

        for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {

            SubRoute* subRoute = &route->subRoutes[subRouteIndex];
            
            int firstCustomerIndex = subRoute->first();
            int lastCustomerIndex = subRoute->last();

            edgesOccurrenceCount[depotVertexIndex][firstCustomerIndex] += 1;
            edgesOccurrenceCount[lastCustomerIndex][depotVertexIndex] += 1;

            edgesCount += 2;

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

void StodolaInspiredAntSystem::localOptimization(Solution& generationBestSolution) {

    Solution newSolution(
        problemInstance.depotsCount,
        problemInstance.minimizationType,
        problemInstance.customersCount
    );

    newSolution.copy(generationBestSolution);

    exchangeMembersInSolution(newSolution);
    moveMembersInSolution(newSolution);

    swap(generationBestSolution, newSolution);
    newSolution.finalize();
}

void StodolaInspiredAntSystem::exchangeMembersInSolution(Solution& exchangeSolution) {

    Route* route;
    Route* exchangeRoute;

    for(int successiveVerticesCount = maxExchangeSuccessiveVertices; successiveVerticesCount > 0; successiveVerticesCount--) {
        for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
            
            route = &exchangeSolution.routes[depotIndex];

            // std::cout << "depotIndex: [" << depotIndex << "] - size: " << route->size << " - " << exchangeRoute->size << "\n";
            exchangeMembersInRoutes(exchangeSolution, *route, *route, successiveVerticesCount);

            for(int exchangeDepotIndex = depotIndex + 1; exchangeDepotIndex < problemInstance.depotsCount; exchangeDepotIndex++) {

                exchangeRoute = &exchangeSolution.routes[exchangeDepotIndex];

                // std::cout << "depotIndex: [" << depotIndex << "] - ";
                // std::cout << "[" << exchangeDepotIndex << "] - ";
                // std::cout << "size: " << route->size << " - " << exchangeRoute->size << "\n";
                exchangeMembersInRoutes(exchangeSolution, *route, *exchangeRoute, successiveVerticesCount);
            }
        }
    }
}

void StodolaInspiredAntSystem::exchangeMembersInRoutes(
    Solution& exchangeSolution, 
    Route& route,
    Route& exchangeRoute,
    int successiveVerticesCount
) {
    
    SubRoute* subRoute;
    SubRoute* exchangeSubRoute;

    int isSameRoute = (route.depotIndex == exchangeRoute.depotIndex);

    for(int subRouteIndex = 0; subRouteIndex < route.size; subRouteIndex++) {

        subRoute = &route.subRoutes[subRouteIndex];
        if(subRoute->length < successiveVerticesCount) {
            break;
        }

        int startExchangeSubRouteIndex = 0;
        if(isSameRoute) {
            startExchangeSubRouteIndex = subRouteIndex + 1;
            exchangeMembersInSameSubRoute(exchangeSolution, *subRoute, successiveVerticesCount);
        }

        for(int exchangeSubRouteIndex = startExchangeSubRouteIndex; exchangeSubRouteIndex < exchangeRoute.size; exchangeSubRouteIndex++) {

            exchangeSubRoute = &exchangeRoute.subRoutes[exchangeSubRouteIndex];
            if(exchangeSubRoute->length < successiveVerticesCount) {
                break;
            }

            // std::cout << "subRoute: [" << subRouteIndex << "] - ";
            // std::cout << "[" << exchangeSubRouteIndex << "] - ";
            // std::cout << "length: " << subRoute->length << " - " << exchangeSubRoute->length << "\n";

            exchangeMembersInDifferentSubRoutes(exchangeSolution, *subRoute, *exchangeSubRoute, successiveVerticesCount);
        }
    }
}

void StodolaInspiredAntSystem::exchangeMembersInSameSubRoute(
    Solution& exchangeSolution, 
    SubRoute& subRoute,
    int successiveVerticesCount
) {

    double baseFitness = exchangeSolution.fitness;

    for(int memberIndex = 0; memberIndex < subRoute.length; memberIndex++) {

        int maxSuccessiveMembersCount = memberIndex + successiveVerticesCount;
        int willExceedSubRoute = ( maxSuccessiveMembersCount > subRoute.length );
        if(willExceedSubRoute) {
            break;
        }

        for(int exchangeMemberIndex = memberIndex + 1; exchangeMemberIndex < subRoute.length; exchangeMemberIndex++) {
            
            int maxSuccessiveExchangeMembersCount = exchangeMemberIndex + successiveVerticesCount;
            int willExceedExchangeSubRoute = ( maxSuccessiveExchangeMembersCount > subRoute.length );
            if(willExceedExchangeSubRoute) {
                break;
            }

            // std::cout << "member: [" << memberIndex << "] - ";
            // std::cout << "[" << exchangeMemberIndex << "]\n";

            subRoute.exchangeMembers(problemInstance, memberIndex, exchangeMemberIndex, successiveVerticesCount);
            exchangeSolution.updateFitness(problemInstance);

            if(exchangeSolution.fitness < baseFitness) {

                // std::cout << "new.fitness: " << exchangeSolution.fitness << " *improved - ";
                baseFitness = exchangeSolution.fitness;
            } else {

                subRoute.revertExchangeMembers(problemInstance, memberIndex, exchangeMemberIndex, successiveVerticesCount);
                exchangeSolution.updateFitness(problemInstance);                 
            }
        }
    }
}

void StodolaInspiredAntSystem::exchangeMembersInDifferentSubRoutes(
    Solution& exchangeSolution, 
    SubRoute& subRoute,
    SubRoute& exchangeSubRoute,
    int successiveVerticesCount
) {

    double baseFitness = exchangeSolution.fitness;

    for(int memberIndex = 0; memberIndex < subRoute.length; memberIndex++) {

        int maxSuccessiveMembersCount = memberIndex + successiveVerticesCount;
        int willExceedSubRoute = ( maxSuccessiveMembersCount > subRoute.length );
        if(willExceedSubRoute) {
            break;
        }

        for(int exchangeMemberIndex = 0; exchangeMemberIndex < exchangeSubRoute.length; exchangeMemberIndex++) {

            int maxSuccessiveExchangeMembersCount = exchangeMemberIndex + successiveVerticesCount;
            int willExceedExchangeSubRoute = ( maxSuccessiveExchangeMembersCount > exchangeSubRoute.length );
            if(willExceedExchangeSubRoute) {
                break;
            }

            // std::cout << "member: [" << memberIndex << "] - ";
            // std::cout << "[" << exchangeMemberIndex << "]\n";

            exchangeMembersBetweenSubRoutes(problemInstance, subRoute, exchangeSubRoute, memberIndex, exchangeMemberIndex, successiveVerticesCount);
            exchangeSolution.updateFitness(problemInstance);

            int isSubRouteContraintSatisfied = subRoute.checkWeightConstraint(problemInstance);
            int isExchangeSubRouteConstraintSatisfied = exchangeSubRoute.checkWeightConstraint(problemInstance);
            int isBetterSolution = (exchangeSolution.fitness < baseFitness);

            if(isSubRouteContraintSatisfied && isExchangeSubRouteConstraintSatisfied && isBetterSolution) {

                // std::cout << "new.fitness: " << exchangeSolution.fitness << " *improved - ";
                baseFitness = exchangeSolution.fitness;
            } else {

                exchangeMembersBetweenSubRoutes(problemInstance, subRoute, exchangeSubRoute, memberIndex, exchangeMemberIndex, successiveVerticesCount);
                exchangeSolution.updateFitness(problemInstance);  
            }
        }
    }
}

void StodolaInspiredAntSystem::moveMembersInSolution(Solution& moveSolution) {

    Route* route;
    Route* destRoute;

    for(int successiveVerticesCount = maxExchangeSuccessiveVertices; successiveVerticesCount > 0; successiveVerticesCount--) {
        for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
            
            route = &moveSolution.routes[depotIndex];

            for(int destDepotIndex = 0; destDepotIndex < problemInstance.depotsCount; destDepotIndex++) {

                destRoute = &moveSolution.routes[destDepotIndex];

                // std::cout << "depotIndex: [" << depotIndex << "] - ";
                // std::cout << "[" << destDepotIndex << "] - ";
                // std::cout << "size: " << route->size << " - " << destRoute->size << "\n";
                moveMembersInRoutes(moveSolution, *route, *destRoute, successiveVerticesCount);
            }
        }
    }
}

void StodolaInspiredAntSystem::moveMembersInRoutes(
    Solution& moveSolution, 
    Route& route,
    Route& destRoute,
    int successiveVerticesCount
) {
    
    SubRoute* subRoute;
    SubRoute* destSubRoute;

    int isSameRoute = (route.depotIndex == destRoute.depotIndex);
    int isSameSubRoute;

    int subRouteIndex = 0;
    int destSubRouteIndex = 0;

    while(subRouteIndex < route.size) {

        subRoute = &route.subRoutes[subRouteIndex];
        if(subRoute->length < successiveVerticesCount) {
            subRouteIndex++;
            continue;
        }

        int hasLeftShiftedSubRoute = 0;
        while(!hasLeftShiftedSubRoute && destSubRouteIndex < destRoute.size) {

            destSubRoute = &destRoute.subRoutes[destSubRouteIndex];

            isSameSubRoute = isSameRoute && (subRoute->subRouteIndex == destSubRoute->subRouteIndex);
            if(isSameSubRoute) {
                destSubRouteIndex++;
                continue;
            }

            // std::cout << "subRoute: [" << subRouteIndex << "] - ";
            // std::cout << "[" << destSubRouteIndex << "] - ";
            // std::cout << "length: " << subRoute->length << " - " << destSubRoute->length << "\n";

            hasLeftShiftedSubRoute = moveMembersInSubRoutes(moveSolution, *subRoute, *destSubRoute, successiveVerticesCount);
            destSubRouteIndex++;
        }

        if(!hasLeftShiftedSubRoute) {
            subRouteIndex++;
        }
    }
}

int StodolaInspiredAntSystem::moveMembersInSubRoutes(
    Solution& moveSolution, 
    SubRoute& subRoute,
    SubRoute& destSubRoute,
    int successiveVerticesCount
) {

    auto willSuccessiveExceedSubRoute = [successiveVerticesCount](int memberIndex, SubRoute& subRoute) {
        return (memberIndex + successiveVerticesCount) > subRoute.length;
    };
    auto willExceedSubRoute = [](int memberIndex, SubRoute& subRoute) {
        return memberIndex > subRoute.length;
    };

    double baseFitness = moveSolution.fitness;
    int memberIndex = 0;
    int destMemberIndex = 0;

    Route* route = &moveSolution.routes[subRoute.depotIndex];
    Route* destRoute = &moveSolution.routes[destSubRoute.depotIndex];

    while( !willSuccessiveExceedSubRoute(memberIndex, subRoute) ) {

        int isBetterSolution = 0;
        while( !isBetterSolution && !willExceedSubRoute(destMemberIndex, destSubRoute) ) {

            // std::cout << "member: [" << memberIndex << "] - ";
            // std::cout << "[" << destMemberIndex << "]\n";

            moveMembersBetweenSubRoutes(problemInstance, subRoute, destSubRoute, memberIndex, destMemberIndex, successiveVerticesCount);

            int hasLeftShiftedSubRoute = (subRoute.length == 0);
            if(hasLeftShiftedSubRoute) {
                // std::cout << "RemovedSubRoute: " << subRoute.subRouteIndex << "\n";
                // std::cout << "Before - route->size: " << route->size << "\n";
                route->shiftLeftSubRoutes(subRoute.subRouteIndex);
                // std::cout << "After - route->size: " << route->size << "\n";
            }

            moveSolution.updateFitness(problemInstance);

            int isSubRouteContraintSatisfied = subRoute.checkWeightConstraint(problemInstance);
            int isDestSubRouteConstraintSatisfied = destSubRoute.checkWeightConstraint(problemInstance);

            isBetterSolution = isSubRouteContraintSatisfied && isDestSubRouteConstraintSatisfied && (moveSolution.fitness < baseFitness);
            if(isBetterSolution) {

                // std::cout << "new.fitness: " << moveSolution.fitness << " *improved\n";
                baseFitness = moveSolution.fitness;

                if(hasLeftShiftedSubRoute) {
                    return 1;
                }

                memberIndex--;
            } else {

                if(hasLeftShiftedSubRoute) {
                    route->shiftRightSubRoutes(subRoute.subRouteIndex);
                }

                moveMembersBetweenSubRoutes(problemInstance, destSubRoute, subRoute, destMemberIndex, memberIndex, successiveVerticesCount);
                moveSolution.updateFitness(problemInstance);

                destMemberIndex++;
            }
        }

        memberIndex++;
    }

    return 0;
}

void StodolaInspiredAntSystem::initializeDronePheromoneMatrices() {

    dronePheromoneMatrix = (double***) malloc(problemInstance.depotsCount * sizeof(double**));

    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
        dronePheromoneMatrix[depotIndex] = (double**) mallocMatrix(problemInstance.verticesCount, problemInstance.customersCount, sizeof(double*), sizeof(double));
        fillMatrix(dronePheromoneMatrix[depotIndex], problemInstance.verticesCount, problemInstance.customersCount, 1.0);
    }
}

void StodolaInspiredAntSystem::buildDroneAntRoutes(Solution& antSolution, int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage) {

    antSolution.reset();

    int unvisitedCustomersCount = problemInstance.customersCount;
    fillArray(visitedCustomersIndexes, unvisitedCustomersCount, 0);

    while(unvisitedCustomersCount > 0) {

        // std::cout << "--------- unvisitedCustomersCount: " << unvisitedCustomersCount << "\n";
        // std::cout << "--------- unvisitedCustomers: ";
        // printIndexesArray(visitedCustomersIndexes, problemInstance.customersCount, 0);

        int depotIndex = selectDepot(visitedCustomersIndexes, selectionProbability, antSolution.routes);
        // std::cout << "------------ depotIndex: " << depotIndex << "\n";

        Route* currentRoute = &antSolution.routes[depotIndex];

        // std::cout << "------------ currentRoute: ";
        // currentRoute->print();
        // std::cout << "\n";

        int currentVertexIndex = currentRoute->last();
        // std::cout << "------------ currentVertexIndex: " << currentVertexIndex << "\n";
        // std::cout << "------------ currentCluster: ";
        // verticesClusters[currentVertexIndex].print(visitedCustomersIndexes);

        int subClusterIndex = selectSubCluster(visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, depotIndex, currentVertexIndex, pheromoneMatrix);
        // std::cout << "------------ subClusterIndex: " << subClusterIndex << "\n";

        int customerIndex = selectCustomer(visitedCustomersIndexes, selectionProbability, depotIndex, currentVertexIndex, subClusterIndex);
        // std::cout << "------------ customerIndex: " << customerIndex << "\n";

        Customer* nextCustomer = &problemInstance.customers[customerIndex];
        Truck* currentTruck = &problemInstance.depots[depotIndex].truck;

        double updatedTruckLoad = ( currentRoute->currentLoad() + nextCustomer->demand );
        bool willTruckExceedCapacity = updatedTruckLoad > currentTruck->capacity;

        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);
        double customerDeliveryDuration = calculateDeliveryDuration(*currentTruck, currentVertexIndex, customerIndex);
        double depotReturnDuration = calculateMovementTime(*currentTruck, customerIndex, depotVertexIndex);
        double updatedTruckDuration = (currentRoute->currentDuration() + customerDeliveryDuration + depotReturnDuration); 
        bool willTruckExceedMaxDuration = updatedTruckDuration > currentTruck->routeMaxDuration;

        if(willTruckExceedCapacity || willTruckExceedMaxDuration) {
            currentRoute->expand();
            //TODO: define logic for drone launch and recovery at depot
        }

        currentRoute->insert(customerIndex);
        currentRoute->incrementCurrentLoad(nextCustomer->demand);

        visitedCustomersIndexes[customerIndex] = 1;
        unvisitedCustomersCount--;

        int droneSubClusterIndex = selectSubCluster(visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, depotIndex, currentVertexIndex, dronePheromoneMatrix);
        int droneCustomerIndex = selectDroneCustomer(visitedCustomersIndexes, selectionProbability, depotIndex, droneSubClusterIndex, currentVertexIndex, customerIndex, *currentRoute);

        if(droneCustomerIndex != -1) {
            DroneRoute* currentDroneRoute = &antSolution.droneRoutes[depotIndex];
            Customer* nextDroneCustomer = &problemInstance.customers[droneCustomerIndex];
            Drone* currentDrone = &problemInstance.depots[depotIndex].drone;

            Sortie sortie(currentVertexIndex, droneCustomerIndex, customerIndex);

            currentDroneRoute->insert(sortie);

            double truckFullDuration = customerDeliveryDuration + currentDrone->launchTime + currentDrone->recoveryTime;
            double droneDeliveryDuration = calculateDroneDeliveryDuration(*currentDrone, sortie);
            double vehicleLongestDuration = max(truckFullDuration, droneDeliveryDuration);
            
            currentRoute->incrementCurrentLoad(nextDroneCustomer->demand);
            currentRoute->incrementCurrentDuration(vehicleLongestDuration);

            visitedCustomersIndexes[droneCustomerIndex] = 1;
            unvisitedCustomersCount--;
        } else {
            currentRoute->incrementCurrentDuration(customerDeliveryDuration);
        }
    }

    antSolution.updateFitness(problemInstance);
}

int StodolaInspiredAntSystem::selectDroneCustomer(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int droneSubClusterIndex, int launchVertexIndex, int recoveryVertexIndex, const Route& route) {

    int droneCustomerIndex = -1;
    double maxPheromoneConcentration = 0;

    Cluster* cluster = &verticesClusters[launchVertexIndex];
    SubCluster* subCluster = &cluster->subClusters[droneSubClusterIndex];

    bool* candidateMembersIndex = (bool*) calloc(subCluster->size, sizeof(bool));
    int candidatesCount = 0;

    Depot* depot = &problemInstance.depots[depotIndex];
    Truck* truck = &depot->truck;
    Drone* drone = &depot->drone;
    
    for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {
        
        int neighborCustomerIndex = subCluster->elements[memberIndex];
        if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

            Sortie sortie(launchVertexIndex, neighborCustomerIndex, recoveryVertexIndex);
            Customer* droneCustomer = &problemInstance.customers[neighborCustomerIndex];

            if(canDroneVisitCustomer(route, sortie, *droneCustomer, *truck, *drone)) {
                candidateMembersIndex[memberIndex] = true;
                candidatesCount++;

                maxPheromoneConcentration = max(
                    maxPheromoneConcentration, 
                    dronePheromoneMatrix[depotIndex][launchVertexIndex][neighborCustomerIndex]
                );
            }
        }
    }

    if(candidatesCount > 0) {

        double droneUsageProbability = 1 - (1 / exp(maxPheromoneConcentration));
        double randomValue = ((double)rand() / RAND_MAX);

        if(randomValue <= droneUsageProbability) {

            updateDroneCustomerSelectionProbability(candidateMembersIndex, selectionProbability, depotIndex, launchVertexIndex, *subCluster);
            
            droneCustomerIndex = rouletteWheelSelection(
                selectionProbability,
                subCluster->size
            );
        }
    }

    return droneCustomerIndex;
}

void StodolaInspiredAntSystem::updateDroneCustomerSelectionProbability(bool* candidateMembersIndex, double* customerSelectionProbability, int depotIndex, int vertexIndex, const SubCluster& subCluster) {

    fillArray(customerSelectionProbability, subCluster.size, 0.0);

    for(int memberIndex = 0; memberIndex < subCluster.size; memberIndex++) {
        
        if(candidateMembersIndex[memberIndex]) {

            int neighborCustomerIndex = subCluster.elements[memberIndex];

            double weightedPheromoneConcentration = weightedValue(
                dronePheromoneMatrix[depotIndex][vertexIndex][neighborCustomerIndex], 
                pheromoneProbabilityCoef
            );

            double weightedDistance = weightedValue(
                problemInstance.verticesDistanceMatrix[vertexIndex][neighborCustomerIndex], 
                distanceProbabilityCoef
            );

            customerSelectionProbability[memberIndex] = weightedPheromoneConcentration;
            customerSelectionProbability[memberIndex] /= weightedDistance;
        }
    }
}

bool StodolaInspiredAntSystem::canDroneVisitCustomer(const Route& route, const Sortie& sortie, const Customer& customer, const Truck& truck, const Drone& drone) {

    if(customer.demand > drone.capacity) {
        return false;
    }

    if(route.currentLoad() + customer.demand > truck.capacity) {
        return false;
    }

    double droneDeliveryDuration = calculateDroneDeliveryDuration(drone, sortie);
    if(droneDeliveryDuration > drone.endurance) {
        return false;
    }

    if(route.currentDuration() + droneDeliveryDuration > truck.routeMaxDuration) {
        return false;
    }

    double customerDeliveryDuration = calculateDeliveryDuration(truck, sortie.launchVertexIndex, sortie.recoveryVertexIndex);
    if(route.currentDuration() + customerDeliveryDuration + drone.launchTime + drone.recoveryTime > drone.endurance) {
        return false;
    }

    return true;
}

double StodolaInspiredAntSystem::calculateDeliveryDuration(const Vehicle& vehicle, int sourceIndex, int destIndex) {

    return calculateMovementTime(vehicle, sourceIndex, destIndex) + vehicle.serviceTime;
}

double StodolaInspiredAntSystem::calculateDroneDeliveryDuration(const Drone& drone, const Sortie& sortie) {

    return calculateMovementTime(drone, sortie.launchVertexIndex, sortie.deliveryVertexIndex) + 
        calculateMovementTime(drone, sortie.deliveryVertexIndex, sortie.recoveryVertexIndex) +
        drone.launchTime +
        drone.recoveryTime +
        drone.serviceTime;
}

double StodolaInspiredAntSystem::calculateMovementTime(const Vehicle& vehicle, int sourceIndex, int destIndex) {

    return problemInstance.verticesDistanceMatrix[sourceIndex][destIndex] / vehicle.speed;
}