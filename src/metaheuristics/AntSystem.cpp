#include "../../include/metaheuristics/AntSystem.hpp"

#include <chrono>
#include <cmath>    // ceil()
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <iostream>

#include "../../include/utils/ArrayUtils.hpp"
#include "../../include/utils/MathUtils.hpp"

void AntSystem::create(int primarySubClustersMaxCount, int subClusterMaxSize) {

    if(distanceProbabilityCoef == 1 && pheromoneProbabilityCoef == 1) {
        weightedValue = [](double value, double weight) { return value; };
    } else {
        weightedValue = [](double value, double weight) { return pow(value, weight); };
    }

    createClusters(primarySubClustersMaxCount, subClusterMaxSize);

    int primarySubClustersCount = verticesClusters[0].primariesCount;

    visitedCustomersIndexes = (int*) malloc(problemInstance.customersCount * sizeof(int));
    selectionProbability = (double*) malloc(problemInstance.customersCount * sizeof(double));
    heuristicInformationAverage = (double*) malloc(primarySubClustersCount * sizeof(double));
    pheromoneConcentrationAverage = (double*) malloc(primarySubClustersCount * sizeof(double));
}

void AntSystem::createClusters(int primarySubClustersMaxCount, int subClusterMaxSize) {

    verticesClusters = (Cluster*) malloc(problemInstance.verticesCount * sizeof(Cluster));

    for(int vertexIndex = 0; vertexIndex < problemInstance.verticesCount; vertexIndex++) {
        verticesClusters[vertexIndex] = Cluster(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, vertexIndex);
    }
}

void AntSystem::finalize() {
    
    problemInstance.finalize();
    frame.finalize();
    pheromone.finalize();
    informationEntropy.finalize();

    antSolution.finalize();
    generationBestSolution.finalize();
    bestSolution.finalize();

    if(visitedCustomersIndexes != nullptr) {
        free(visitedCustomersIndexes);
    }

    if(selectionProbability != nullptr) {
        free(selectionProbability);
    }

    if(heuristicInformationAverage != nullptr) {
        free(heuristicInformationAverage);
    }

    if(pheromoneConcentrationAverage != nullptr) {
        free(pheromoneConcentrationAverage);
    }

    if(verticesClusters != nullptr) {

        for(int vertexIndex = 0; vertexIndex < problemInstance.verticesCount; vertexIndex++) {
            verticesClusters[vertexIndex].finalize();
        }

        free(verticesClusters);
    }
}

void AntSystem::run() {

    std::chrono::time_point startOptimizationTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point endOptimizationTime = startOptimizationTime;
    std::chrono::duration<double> currentOptimizationTime = endOptimizationTime - startOptimizationTime;
    
    int globalImprovementsCount = 0;
    int iterationsCount = 0;
    int iterationsWithoutImprovementCount = 0;

    Solution* consideredSolution = nullptr;

    //initial solution
    buildAntRoutes(bestSolution);

    while(!hasAchievedTerminationCondition(
        iterationsCount, 
        iterationsWithoutImprovementCount,
        currentOptimizationTime.count(),
        informationEntropy.coef
    )) 
    {

        // std::cout << "--- generation: " << iterationsCount << "\n";
        informationEntropy.resetEdgesValues();

        //first ant
        // std::cout << "------ ant: " << 0 << "\n";
        buildAntRoutes(generationBestSolution);
        informationEntropy.updateEdgesOccurrence(generationBestSolution);
        
        //others ants
        for(int antIndex = 1; antIndex < antsCount; antIndex++) {

            // std::cout << "------ ant: " << antIndex << "\n";

            buildAntRoutes(antSolution);
            informationEntropy.updateEdgesOccurrence(antSolution);

            if(antSolution.fitness < generationBestSolution.fitness) {
                std::swap(generationBestSolution, antSolution);
            }
        }

        //TODO: local optimization inter-route exchange
        if(iterationsCount % localOptimizationFrequency == 0) {
            localOptimization.run(generationBestSolution);
        }

        consideredSolution = &bestSolution;
        if(generationBestSolution.fitness < bestSolution.fitness) {

            std::swap(bestSolution, generationBestSolution);

            globalImprovementsCount += 1;

            std::cout << "--------------------------------------------------\n";
            std::cout << "globalImproves: " << globalImprovementsCount << " - ";
            std::cout << "fitness: " << bestSolution.fitness << " - ";
            std::cout << "generations: " << iterationsCount << " - ";
            std::cout << "generationsUntilImprove: " << iterationsWithoutImprovementCount  << " - ";

            iterationsWithoutImprovementCount = 0;
        } else if(canUseWorseSolution(generationBestSolution)) {
            consideredSolution = &generationBestSolution;
        }

        pheromone.reinforceMatrices(bestSolution, *consideredSolution);
        coolDownTemperature();
        informationEntropy.update();
        pheromone.evaporateMatrices(informationEntropy);

        endOptimizationTime = std::chrono::high_resolution_clock::now();
        currentOptimizationTime = endOptimizationTime - startOptimizationTime;

        if(iterationsWithoutImprovementCount == 0) {

            std::cout << "timer: " << currentOptimizationTime.count() << "\n";
            std::cout << "informationEntropy:" << " ";
            std::cout << "normalized: " << informationEntropy.normalized << " - ";
            std::cout << "coef: " << informationEntropy.coef << " - ";
            std::cout << "min: " << informationEntropy.min << " - ";
            std::cout << "cur: " << informationEntropy.cur << " - ";
            std::cout << "max: " << informationEntropy.max << "\n";
        }

        iterationsCount += 1;
        iterationsWithoutImprovementCount += 1;
    }

    std::cout << "--------------------------------------------------\n";
    std::cout << "generations: " << iterationsCount << " - ";
    std::cout << "generationsWithoutImprove: " << iterationsWithoutImprovementCount  << " - ";
    std::cout << "timer: " << currentOptimizationTime.count() << "\n";
    
    bestSolution.print();
}

void AntSystem::buildAntRoutes(Solution& antSolution) {

    antSolution.reset();

    int unvisitedCustomersCount = problemInstance.customersCount;
    fillArray(visitedCustomersIndexes, unvisitedCustomersCount, 0);

    while(unvisitedCustomersCount > 0) {

        // std::cout << "--------- unvisitedCustomersCount: " << unvisitedCustomersCount << "\n";
        // std::cout << "--------- unvisitedCustomers: ";
        // printIndexesArray(visitedCustomersIndexes, problemInstance.customersCount, 0);

        int depotIndex = selectDepot(antSolution.routes);
        // std::cout << "------------ depotIndex: " << depotIndex << "\n";

        Route* currentRoute = &antSolution.routes[depotIndex];
        SubRoute* currentSubRoute = &antSolution.routes[depotIndex].last();

        // std::cout << "------------ currentRoute: ";
        // currentRoute->print();
        // std::cout << "\n";

        int currentVertexIndex = currentSubRoute->last();
        // std::cout << "------------ currentVertexIndex: " << currentVertexIndex << "\n";
        // std::cout << "------------ currentCluster: ";
        // verticesClusters[currentVertexIndex].print(visitedCustomersIndexes);

        int subClusterIndex = selectSubCluster(depotIndex, currentVertexIndex, pheromone.truckMatrices);
        // std::cout << "------------ subClusterIndex: " << subClusterIndex << "\n";

        int customerIndex = selectCustomer(depotIndex, currentVertexIndex, subClusterIndex);
        // std::cout << "------------ customerIndex: " << customerIndex << "\n";

        Customer* nextCustomer = &problemInstance.customers[customerIndex];
        Truck* currentTruck = &problemInstance.depots[depotIndex].truck;

        double updatedTruckLoad = ( currentSubRoute->load + nextCustomer->demand );
        bool willTruckExceedCapacity = updatedTruckLoad > currentTruck->capacity;

        if(willTruckExceedCapacity) {
            currentRoute->expand();
            currentSubRoute = &currentRoute->last();
        }

        currentSubRoute->insert(customerIndex);
        currentSubRoute->incrementLoad(nextCustomer->demand);

        visitedCustomersIndexes[customerIndex] = 1;
        unvisitedCustomersCount--;
    }

    antSolution.updateFitness();
}

int AntSystem::selectDepot(Route* routes) {

    int selectedDepotIndex = -1;

    int hasUnvisitedCustomers = updateDepotSelectionProbability(routes);
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

int AntSystem::updateDepotSelectionProbability(Route* routes) {

    fillArray(selectionProbability, problemInstance.depotsCount, 0.0);

    int consideredCustomersCount = 0;
    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {

        SubRoute* subRoute = &routes[depotIndex].last();
        int vertexIndex = subRoute->last();
        Cluster* cluster = &verticesClusters[vertexIndex];

        for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {

            SubCluster* subCluster = &cluster->subClusters[subClusterIndex];
            for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {

                int neighborCustomerIndex = subCluster->elements[memberIndex];
                if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

                    consideredCustomersCount++;
                    selectionProbability[depotIndex] += pheromone.truckMatrices[depotIndex][vertexIndex][neighborCustomerIndex];
                }
            }
        }
    }
    
    return consideredCustomersCount;
}

int AntSystem::selectSubCluster(int depotIndex, int vertexIndex, double*** pheromoneMatrix) {
    
    int selectedSubClusterIndex = -1;

    int hasUnvisitedCustomers = updatePrimarySubClusterSelectionProbability(depotIndex, vertexIndex, pheromoneMatrix);
    if(hasUnvisitedCustomers) {

        Cluster* cluster = &verticesClusters[vertexIndex];
        selectedSubClusterIndex = rouletteWheelSelection(
            selectionProbability,
            cluster->primariesCount
        );
    } else { //if there is no customer unvisited for all primary clusters, choose non primary cluster
        selectedSubClusterIndex = selectSubClusterNonPrimary(vertexIndex);
    }

    return selectedSubClusterIndex;
}

int AntSystem::updatePrimarySubClusterSelectionProbability(int depotIndex, int vertexIndex, double*** pheromoneMatrix) {

    Cluster* cluster = &verticesClusters[vertexIndex];
    
    fillArray(pheromoneConcentrationAverage, cluster->primariesCount, 0.0);
    fillArray(heuristicInformationAverage, cluster->primariesCount, 0.0);
    fillArray(selectionProbability, cluster->primariesCount, 0.0);

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
        calculatePrimarySubClusterSelectionProbability(cluster);
    }

    return consideredCustomersCountSum;
}

void AntSystem::calculatePrimarySubClusterSelectionProbability(Cluster* cluster) {

    for(int subClusterIndex = 0; subClusterIndex < cluster->primariesCount; subClusterIndex++) {
        
        double weightedHeuristicInformationAverage = weightedValue(
            heuristicInformationAverage[subClusterIndex], 
            distanceProbabilityCoef
        );

        double weightedPheromoneConcentrationAverage = weightedValue(
            pheromoneConcentrationAverage[subClusterIndex], 
            pheromoneProbabilityCoef
        );

        selectionProbability[subClusterIndex] = weightedHeuristicInformationAverage;
        selectionProbability[subClusterIndex] *= weightedPheromoneConcentrationAverage;
    }
}

int AntSystem::selectSubClusterNonPrimary(int vertexIndex) {
    
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

int AntSystem::selectCustomer(int depotIndex, int vertexIndex, int subClusterIndex) {

    SubCluster* subCluster = &verticesClusters[vertexIndex].subClusters[subClusterIndex];

    updateCustomerSelectionProbability(depotIndex, vertexIndex, *subCluster);

    int memberIndex = rouletteWheelSelection(
        selectionProbability,
        subCluster->size
    );

    return subCluster->elements[memberIndex];
}

void AntSystem::updateCustomerSelectionProbability(int depotIndex, int vertexIndex, const SubCluster& subCluster) {

    fillArray(selectionProbability, subCluster.size, 0.0);

    for(int memberIndex = 0; memberIndex < subCluster.size; memberIndex++) {
        
        int neighborCustomerIndex = subCluster.elements[memberIndex];
        if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

            double weightedPheromoneConcentration = weightedValue(
                pheromone.truckMatrices[depotIndex][vertexIndex][neighborCustomerIndex], 
                pheromoneProbabilityCoef
            );

            double weightedDistance = weightedValue(
                problemInstance.verticesDistanceMatrix[vertexIndex][neighborCustomerIndex], 
                distanceProbabilityCoef
            );

            selectionProbability[memberIndex] = weightedPheromoneConcentration;
            selectionProbability[memberIndex] /= weightedDistance;
        }
    }
}

bool AntSystem::hasAchievedTerminationCondition(int iterationsCount, int iterationsWithoutImprovementCount, double currentOptimizationTime, double informationEntropyCoef) {

    return (iterationsCount >= maxIterations) ||
        (iterationsWithoutImprovementCount >= maxIterationsWithoutImprovement) ||
        (currentOptimizationTime >= maxOptimizationTime) ||
        (informationEntropyCoef < minInformationEntropyCoef);
}

bool AntSystem::canUseWorseSolution(const Solution& generationBestSolution) {

    double bestSolutionProbability = generationBestSolution.fitness - bestSolution.fitness;
    bestSolutionProbability /= bestSolution.fitness;
    bestSolutionProbability /= temperature;
    bestSolutionProbability *= -1;
    bestSolutionProbability = exp(bestSolutionProbability);

    double generationBestSolutionProbability = 1 - bestSolutionProbability;
    double randomValue = ((double)rand() / RAND_MAX);

    return (randomValue <= generationBestSolutionProbability);
}
