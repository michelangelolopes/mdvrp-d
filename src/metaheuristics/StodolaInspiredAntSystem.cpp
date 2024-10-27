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

void StodolaInspiredAntSystem::updatePheromoneMatrix(const Solution& consideredSolution, double updateValue, int isSumOperation) {

    std::function<double(double, double)> operationFunction;
    
    if(isSumOperation) {
        operationFunction = [](double x, double y) { return x + y; };
    } else {
        operationFunction = [](double x, double y) { return x * y; };
    }

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
    int** generationEdgesOccurrenceCount = (int**) mallocMatrix(problemInstance.verticesCount, problemInstance.verticesCount, sizeof(int*), sizeof(int));
    int* visitedCustomersIndexes = (int*) malloc(problemInstance.customersCount * sizeof(int));
    double* selectionProbability = (double*) malloc(problemInstance.customersCount * sizeof(double));

    int primarySubClustersCout = verticesClusters[0].primariesCount;
    double* heuristicInformationAverage = (double*) malloc(primarySubClustersCout * sizeof(double));
    double* pheromoneConcentrationAverage = (double*) malloc(primarySubClustersCout * sizeof(double));
    
    int globalImprovementsCount = 0;
    int intervalImprovementsCount = 0;

    std::chrono::time_point startOptimizationTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point endOptimizationTime = startOptimizationTime;

    int iterationsCount = 0;
    int iterationsWithoutImprovementCount = 0;
    int oldIterationIndex = 0;
    std::chrono::duration<double> currentOptimizationTime = endOptimizationTime - startOptimizationTime;

    double informationEntropy = -1;
    double informationEntropyMin = -1;
    double informationEntropyMax = -1;
    double informationEntropyCoef = -1;

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

        //TODO: local optimization
        // if(iterIndex % localOptimizationFrequency == 0) {
        // }

        if(generationBestSolution.fitness < bestSolution.fitness) {

            swap(bestSolution, generationBestSolution);
            reinforcePheromoneMatrix(bestSolution);

            intervalImprovementsCount += 1;
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

        iterationsCount += 1;
        iterationsWithoutImprovementCount += 1;
        generationEdgesCount = 0;

        endOptimizationTime = std::chrono::high_resolution_clock::now();
        currentOptimizationTime = endOptimizationTime - startOptimizationTime;

        if(iterationsWithoutImprovementCount >= 50 && intervalImprovementsCount > 0) {

            globalImprovementsCount += intervalImprovementsCount;

            std::cout << "iterationsCount: " << iterationsCount << "\n";
            std::cout << "iterationsWithoutImprovementCount: " << iterationsWithoutImprovementCount << "\n";
            std::cout << "iterationsBetweenIntervals: " << iterationsCount - oldIterationIndex << "\n";
            std::cout << "currentOptimizationTime: " << currentOptimizationTime.count() << "\n";
            std::cout << "globalImprovements: " << globalImprovementsCount << " - ";
            std::cout << "intervalImprovements: " << intervalImprovementsCount << "\n";
            std::cout << "informationEntropy: " << informationEntropy << " - ";
            std::cout << "informationEntropyCoef: " << informationEntropyCoef << "\n";
            std::cout << "informationEntropyMin: " << informationEntropyMin << " - ";
            std::cout << "informationEntropyMax: " << informationEntropyMax << "\n";
            
            bestSolution.print();
            
            oldIterationIndex = iterationsCount;
            intervalImprovementsCount = 0;
        }
    }

    std::cout << "iterationsCount: " << iterationsCount << "\n";
    std::cout << "iterationsWithoutImprovementCount: " << iterationsWithoutImprovementCount << "\n";
    std::cout << "currentOptimizationTime: " << currentOptimizationTime.count() << "\n";
    std::cout << "globalImprovements: " << globalImprovementsCount << " - ";
    std::cout << "intervalImprovements: " << intervalImprovementsCount << "\n";
    // std::cout << "globalImprovements: " << intervalImprovementsCount << "\n";
    std::cout << "informationEntropy: " << informationEntropy << " - ";
    std::cout << "informationEntropyCoef: " << informationEntropyCoef << "\n";
    std::cout << "informationEntropyMin: " << informationEntropyMin << " - ";
    std::cout << "informationEntropyMax: " << informationEntropyMax << "\n";
    
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

        int subClusterIndex = selectSubCluster(visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, depotIndex, currentVertexIndex);
        // std::cout << "------------ subClusterIndex: " << subClusterIndex << "\n";

        int customerIndex = selectCustomer(visitedCustomersIndexes, selectionProbability, depotIndex, currentVertexIndex, subClusterIndex);
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

int StodolaInspiredAntSystem::selectSubCluster(int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage, int depotIndex, int vertexIndex) {
    
    int selectedSubClusterIndex = -1;

    int hasUnvisitedCustomers = updatePrimarySubClusterSelectionProbability(visitedCustomersIndexes, selectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, depotIndex, vertexIndex);
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
    int vertexIndex
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
        if(distanceProbabilityCoef == 1 && pheromoneProbabilityCoef == 1) {
            calculatePrimarySubClusterSelectionProbabilityWithCoefOne(primarySubClusterSelectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, cluster);
        } else {
            calculatePrimarySubClusterSelectionProbability(primarySubClusterSelectionProbability, heuristicInformationAverage, pheromoneConcentrationAverage, cluster);
        }
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
        
        // std::cout << "subCluster[" << subClusterIndex << "]: ";
        // std::cout << heuristicInformationAverage[subClusterIndex] << " - ";
        // std::cout << pheromoneConcentrationAverage[subClusterIndex] << "\n";
        primarySubClusterSelectionProbability[subClusterIndex] = pow(heuristicInformationAverage[subClusterIndex], distanceProbabilityCoef);
        primarySubClusterSelectionProbability[subClusterIndex] *= pow(pheromoneConcentrationAverage[subClusterIndex], pheromoneProbabilityCoef);
    }
}

void StodolaInspiredAntSystem::calculatePrimarySubClusterSelectionProbabilityWithCoefOne( 
    double* primarySubClusterSelectionProbability,
    double* heuristicInformationAverage, 
    double* pheromoneConcentrationAverage,
    Cluster* cluster
) {
    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {
        
        // std::cout << "subCluster[" << subClusterIndex << "]: ";
        // std::cout << heuristicInformationAverage[subClusterIndex] << " - ";
        // std::cout << pheromoneConcentrationAverage[subClusterIndex] << "\n";
        primarySubClusterSelectionProbability[subClusterIndex] = heuristicInformationAverage[subClusterIndex];
        primarySubClusterSelectionProbability[subClusterIndex] *= pheromoneConcentrationAverage[subClusterIndex];
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

    if(distanceProbabilityCoef == 1 && pheromoneProbabilityCoef == 1) {
        updateCustomerSelectionProbabilityWithCoefOne(visitedCustomersIndexes, selectionProbability, depotIndex, vertexIndex, *subCluster);
    } else {
        updateCustomerSelectionProbability(visitedCustomersIndexes, selectionProbability, depotIndex, vertexIndex, *subCluster);
    }

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

            customerSelectionProbability[memberIndex] = pow(pheromoneMatrix[depotIndex][vertexIndex][neighborCustomerIndex], pheromoneProbabilityCoef);
            customerSelectionProbability[memberIndex] /= pow(problemInstance.verticesDistanceMatrix[vertexIndex][neighborCustomerIndex], distanceProbabilityCoef);
        }
    }
}

void StodolaInspiredAntSystem::updateCustomerSelectionProbabilityWithCoefOne(int* visitedCustomersIndexes, double* customerSelectionProbability, int depotIndex, int vertexIndex, const SubCluster& subCluster) {
  
    fillArray(customerSelectionProbability, subCluster.size, 0.0);

    for(int memberIndex = 0; memberIndex < subCluster.size; memberIndex++) {
        
        int neighborCustomerIndex = subCluster.elements[memberIndex];
        if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

            customerSelectionProbability[memberIndex] = pheromoneMatrix[depotIndex][vertexIndex][neighborCustomerIndex];
            customerSelectionProbability[memberIndex] /= problemInstance.verticesDistanceMatrix[vertexIndex][neighborCustomerIndex];
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