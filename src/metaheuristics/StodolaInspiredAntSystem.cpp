#include "../../include/metaheuristics/StodolaInspiredAntSystem.hpp"

#include <chrono>
#include <cmath>    // ceil()
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
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

    int primarySubClustersCount = verticesClusters[0].primariesCount;

    visitedCustomersIndexes = (int*) malloc(problemInstance.customersCount * sizeof(int));
    selectionProbability = (double*) malloc(problemInstance.customersCount * sizeof(double));
    heuristicInformationAverage = (double*) malloc(primarySubClustersCount * sizeof(double));
    pheromoneConcentrationAverage = (double*) malloc(primarySubClustersCount * sizeof(double));
}

void StodolaInspiredAntSystem::createClusters(int primarySubClustersMaxCount, int subClusterMaxSize) {

    verticesClusters = (Cluster*) malloc(problemInstance.verticesCount * sizeof(Cluster));

    for(int vertexIndex = 0; vertexIndex < problemInstance.verticesCount; vertexIndex++) {
        verticesClusters[vertexIndex] = Cluster(problemInstance, frame, primarySubClustersMaxCount, subClusterMaxSize, vertexIndex);
    }
}

void StodolaInspiredAntSystem::finalize() {
    
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

void StodolaInspiredAntSystem::run() {

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
        informationEntropy.resetTruckEdgesValues();

        //first ant
        // std::cout << "------ ant: " << 0 << "\n";
        buildAntRoutes(generationBestSolution);
        informationEntropy.updateTruckEdgesOccurrence(generationBestSolution);
        
        //others ants
        for(int antIndex = 1; antIndex < antsCount; antIndex++) {

            // std::cout << "------ ant: " << antIndex << "\n";

            buildAntRoutes(antSolution);
            informationEntropy.updateTruckEdgesOccurrence(antSolution);

            if(antSolution.fitness < generationBestSolution.fitness) {
                swap(generationBestSolution, antSolution);
            }
        }

        //TODO: local optimization inter-route exchange
        if(iterationsCount % localOptimizationFrequency == 0) {
            localOptimization.run(generationBestSolution);
        }

        consideredSolution = &bestSolution;
        if(generationBestSolution.fitness < bestSolution.fitness) {

            swap(bestSolution, generationBestSolution);

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

        pheromone.updateReinforcementValue(bestSolution, *consideredSolution);
        pheromone.reinforceTruckMatrices(*consideredSolution);
        coolDownTemperature();

        informationEntropy.update();

        pheromone.updateEvaporationValue(informationEntropy);
        pheromone.evaporateTruckMatrices();

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

void StodolaInspiredAntSystem::runWithDrone() {

    std::chrono::time_point startOptimizationTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point endOptimizationTime = startOptimizationTime;
    std::chrono::duration<double> currentOptimizationTime = endOptimizationTime - startOptimizationTime;
    
    int globalImprovementsCount = 0;
    int iterationsCount = 0;
    int iterationsWithoutImprovementCount = 0;

    Solution* consideredSolution = nullptr;

    //initial solution
    buildAntRoutesWithDrone(bestSolution);

    while(!hasAchievedTerminationCondition(
        iterationsCount, 
        iterationsWithoutImprovementCount,
        currentOptimizationTime.count(),
        informationEntropy.coef
    )) 
    {

        // std::cout << "--- generation: " << iterationsCount << "\n";
        informationEntropy.resetTruckEdgesValues();
        informationEntropy.resetDroneEdgesValues();

        //first ant
        // std::cout << "------ ant: " << 0 << "\n";
        buildAntRoutesWithDrone(generationBestSolution);
        informationEntropy.updateTruckEdgesOccurrence(generationBestSolution);
        informationEntropy.updateDroneEdgesOccurrence(generationBestSolution);
        
        //others ants
        for(int antIndex = 1; antIndex < antsCount; antIndex++) {

            // std::cout << "------ ant: " << antIndex << "\n";

            buildAntRoutesWithDrone(antSolution);
            informationEntropy.updateTruckEdgesOccurrence(antSolution);
            informationEntropy.updateDroneEdgesOccurrence(antSolution);

            if(antSolution.fitness < generationBestSolution.fitness) {
                swap(generationBestSolution, antSolution);
            }
        }

        // //TODO: local optimization inter-route exchange
        // if(iterationsCount % localOptimizationFrequency == 0) {
        //     localOptimization(generationBestSolution);
        // }

        consideredSolution = &bestSolution;
        if(generationBestSolution.fitness < bestSolution.fitness) {

            swap(bestSolution, generationBestSolution);

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

        pheromone.updateReinforcementValue(bestSolution, *consideredSolution);
        pheromone.reinforceTruckMatrices(*consideredSolution);
        pheromone.reinforceDroneMatrices(*consideredSolution);
        coolDownTemperature();

        informationEntropy.updateWithDrone();

        pheromone.updateEvaporationValue(informationEntropy);
        pheromone.evaporateTruckMatrices();
        pheromone.evaporateDroneMatrices();

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
    
    bestSolution.printWithDrone();

    cout << "checkConstraints: " << bestSolution.checkConstraints() << endl;
}

void StodolaInspiredAntSystem::buildAntRoutes(Solution& antSolution) {

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

        // std::cout << "------------ currentRoute: ";
        // currentRoute->print();
        // std::cout << "\n";

        int currentVertexIndex = currentRoute->last();
        // std::cout << "------------ currentVertexIndex: " << currentVertexIndex << "\n";
        // std::cout << "------------ currentCluster: ";
        // verticesClusters[currentVertexIndex].print(visitedCustomersIndexes);

        int subClusterIndex = selectSubCluster(depotIndex, currentVertexIndex, pheromone.truckMatrices);
        // std::cout << "------------ subClusterIndex: " << subClusterIndex << "\n";

        int customerIndex = selectCustomer(depotIndex, currentVertexIndex, subClusterIndex);
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

    antSolution.updateFitness();
}

void StodolaInspiredAntSystem::buildAntRoutesWithDrone(Solution& antSolution) {

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

        // std::cout << "------------ currentRoute: ";
        // currentRoute->print();
        // antSolution.printWithDrone(depotIndex);
        // std::cout << "\n";

        int currentVertexIndex = currentRoute->last();
        // std::cout << "------------ currentVertexIndex: " << currentVertexIndex << "\n";
        // std::cout << "------------ currentCluster: ";
        // verticesClusters[currentVertexIndex].print(visitedCustomersIndexes);

        int subClusterIndex = selectSubCluster(depotIndex, currentVertexIndex, pheromone.truckMatrices);
        // std::cout << "------------ subClusterIndex: " << subClusterIndex << "\n";

        int customerIndex = selectCustomer(depotIndex, currentVertexIndex, subClusterIndex);
        // std::cout << "------------ customerIndex: " << customerIndex << "\n";

        Customer* nextCustomer = &problemInstance.customers[customerIndex];
        Truck* currentTruck = &problemInstance.depots[depotIndex].truck;

        double updatedTruckLoad = ( currentRoute->currentLoad() + nextCustomer->demand );
        bool willTruckExceedCapacity = updatedTruckLoad > currentTruck->capacity;

        bool willTruckExceedMaxDuration = false;
        double customerDeliveryDuration = problemInstance.calculateDeliveryDuration(*currentTruck, currentVertexIndex, customerIndex);
        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);

        if(currentTruck->routeMaxDuration > 0) {

            double depotReturnDuration = problemInstance.calculateMovementDuration(*currentTruck, customerIndex, depotVertexIndex);
            double updatedTruckDuration = (currentRoute->currentDuration() + customerDeliveryDuration + depotReturnDuration);
            
            willTruckExceedMaxDuration = updatedTruckDuration > currentTruck->routeMaxDuration;
        }

        if(willTruckExceedCapacity || willTruckExceedMaxDuration) {

            double depotReturnDuration = problemInstance.calculateMovementDuration(*currentTruck, currentVertexIndex, depotVertexIndex);
            bool canDroneDeliver = (unvisitedCustomersCount > 0);
            bool hasDroneDelivered = false;
            if(canDroneDeliver) {

                int droneSubClusterIndex = selectSubCluster(depotIndex, currentVertexIndex, pheromone.droneMatrices);
                // std::cout << "------------ droneSubClusterIndex: " << droneSubClusterIndex << "\n";
                int droneCustomerIndex = selectDroneCustomer(depotIndex, droneSubClusterIndex, currentVertexIndex, depotVertexIndex, *currentRoute);
                // std::cout << "------------ droneCustomerIndex: " << droneCustomerIndex << "\n";

                hasDroneDelivered = (droneCustomerIndex != -1);
                if(hasDroneDelivered) {
                    DroneRoute* currentDroneRoute = &antSolution.droneRoutes[depotIndex];
                    Customer* nextDroneCustomer = &problemInstance.customers[droneCustomerIndex];
                    Drone* currentDrone = &problemInstance.depots[depotIndex].drone;

                    Sortie sortie(currentVertexIndex, droneCustomerIndex, depotVertexIndex);

                    // sortie.print();
                    // cout << endl;                    

                    currentDroneRoute->insert(sortie);

                    double truckFullDuration = depotReturnDuration + currentDrone->launchTime + currentDrone->recoveryTime;
                    double droneDeliveryDuration = problemInstance.calculateDeliveryDuration(*currentDrone, sortie);

                    double vehicleLongestDuration = max(truckFullDuration, droneDeliveryDuration);
                    
                    currentRoute->incrementCurrentLoad(nextDroneCustomer->demand);
                    currentRoute->incrementCurrentDuration(vehicleLongestDuration);

                    visitedCustomersIndexes[droneCustomerIndex] = 1;
                    unvisitedCustomersCount--;
                }
            }

            if(!canDroneDeliver || !hasDroneDelivered) {
                currentRoute->incrementCurrentDuration(depotReturnDuration);
            }

            currentRoute->expand();
            currentVertexIndex = depotVertexIndex;
        }

        currentRoute->insert(customerIndex);
        currentRoute->incrementCurrentLoad(nextCustomer->demand);

        visitedCustomersIndexes[customerIndex] = 1;
        unvisitedCustomersCount--;

        bool canDroneDeliver = (unvisitedCustomersCount > 0);
        bool hasDroneDelivered = false;
        customerDeliveryDuration = problemInstance.calculateDeliveryDuration(*currentTruck, currentVertexIndex, customerIndex);
        if(canDroneDeliver) {

            int droneSubClusterIndex = selectSubCluster(depotIndex, currentVertexIndex, pheromone.droneMatrices);
            // std::cout << "------------ droneSubClusterIndex: " << droneSubClusterIndex << "\n";
            int droneCustomerIndex = selectDroneCustomer(depotIndex, droneSubClusterIndex, currentVertexIndex, customerIndex, *currentRoute);
            // std::cout << "------------ droneCustomerIndex: " << droneCustomerIndex << "\n";
            hasDroneDelivered = (droneCustomerIndex != -1);
            if(hasDroneDelivered) {
                DroneRoute* currentDroneRoute = &antSolution.droneRoutes[depotIndex];
                Customer* nextDroneCustomer = &problemInstance.customers[droneCustomerIndex];
                Drone* currentDrone = &problemInstance.depots[depotIndex].drone;

                Sortie sortie(currentVertexIndex, droneCustomerIndex, customerIndex);

                // sortie.print();
                // cout << endl;

                currentDroneRoute->insert(sortie);
                
                double truckFullDuration = customerDeliveryDuration + currentDrone->launchTime + currentDrone->recoveryTime;
                double droneDeliveryDuration = problemInstance.calculateDeliveryDuration(*currentDrone, sortie);

                double vehicleLongestDuration = max(truckFullDuration, droneDeliveryDuration);
                
                currentRoute->incrementCurrentLoad(nextDroneCustomer->demand);
                currentRoute->incrementCurrentDuration(vehicleLongestDuration);

                visitedCustomersIndexes[droneCustomerIndex] = 1;
                unvisitedCustomersCount--;
            }
        }

        if(!canDroneDeliver || !hasDroneDelivered) {
            currentRoute->incrementCurrentDuration(customerDeliveryDuration);
        }
    }

    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);
        Route* currentRoute = &antSolution.routes[depotIndex];
        Truck* currentTruck = &problemInstance.depots[depotIndex].truck;
        double depotReturnDuration = problemInstance.calculateMovementDuration(*currentTruck, currentRoute->last(), depotVertexIndex);
        currentRoute->incrementCurrentDuration(depotReturnDuration);
    }


    antSolution.updateFitnessWithDrone();
}

int StodolaInspiredAntSystem::selectDepot(Route* routes) {

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

int StodolaInspiredAntSystem::updateDepotSelectionProbability(Route* routes) {

    fillArray(selectionProbability, problemInstance.depotsCount, 0.0);

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
                    selectionProbability[depotIndex] += pheromone.truckMatrices[depotIndex][vertexIndex][neighborCustomerIndex];
                }
            }
        }
    }
    
    return consideredCustomersCount;
}

int StodolaInspiredAntSystem::selectSubCluster(int depotIndex, int vertexIndex, double*** pheromoneMatrix) {
    
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

int StodolaInspiredAntSystem::updatePrimarySubClusterSelectionProbability(int depotIndex, int vertexIndex, double*** pheromoneMatrix) {

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

void StodolaInspiredAntSystem::calculatePrimarySubClusterSelectionProbability(Cluster* cluster) {

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

int StodolaInspiredAntSystem::selectSubClusterNonPrimary(int vertexIndex) {
    
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

int StodolaInspiredAntSystem::selectCustomer(int depotIndex, int vertexIndex, int subClusterIndex) {

    SubCluster* subCluster = &verticesClusters[vertexIndex].subClusters[subClusterIndex];

    updateCustomerSelectionProbability(depotIndex, vertexIndex, *subCluster);

    int memberIndex = rouletteWheelSelection(
        selectionProbability,
        subCluster->size
    );

    return subCluster->elements[memberIndex];
}

void StodolaInspiredAntSystem::updateCustomerSelectionProbability(int depotIndex, int vertexIndex, const SubCluster& subCluster) {

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

int StodolaInspiredAntSystem::selectDroneCustomer(int depotIndex, int droneSubClusterIndex, int launchVertexIndex, int recoveryVertexIndex, const Route& route) {

    int droneCustomerIndex = -1;
    double maxPheromoneConcentration = 0;

    Cluster* cluster = &verticesClusters[launchVertexIndex];
    SubCluster* subCluster = &cluster->subClusters[droneSubClusterIndex];

    bool* candidateMembersIndex = (bool*) malloc(subCluster->size * sizeof(bool));
    int candidatesCount = 0;

    Depot* depot = &problemInstance.depots[depotIndex];
    Truck* truck = &depot->truck;
    Drone* drone = &depot->drone;
    
    for(int memberIndex = 0; memberIndex < subCluster->size; memberIndex++) {
        
        candidateMembersIndex[memberIndex] = false;
        int neighborCustomerIndex = subCluster->elements[memberIndex];
        if(visitedCustomersIndexes[neighborCustomerIndex] != 1) {

            Sortie sortie(launchVertexIndex, neighborCustomerIndex, recoveryVertexIndex);
            Customer* droneCustomer = &problemInstance.customers[neighborCustomerIndex];

            if(canDroneVisitCustomer(route, sortie, *droneCustomer, *truck, *drone)) {
                candidateMembersIndex[memberIndex] = true;
                candidatesCount++;

                // std::cout << "pheromoneConcentration: " << pheromone.droneMatrices[depotIndex][launchVertexIndex][neighborCustomerIndex] << "\n";
                maxPheromoneConcentration = max(
                    maxPheromoneConcentration, 
                    pheromone.droneMatrices[depotIndex][launchVertexIndex][neighborCustomerIndex]
                );
            }
        }
    }

    // std::cout << "candidatesCount: " << candidatesCount << "\n";
    if(candidatesCount > 0) {
        
        double droneUsageProbability = 1 - (1 / exp(maxPheromoneConcentration));
        double randomValue = ((double)rand() / RAND_MAX);
        int selectedMemberIndex = -1;
        // std::cout << "launchVertexIndex: " << launchVertexIndex << endl;
        // std::cout << "recoveryVertexIndex: " << recoveryVertexIndex << endl;
        // std::cout << "maxPheromoneConcentration: " << maxPheromoneConcentration << endl;
        // std::cout << "droneUsageProbability: " << droneUsageProbability << endl;
        // std::cout << "randomValue: " << randomValue << endl;

        if(randomValue <= droneUsageProbability) {

            updateDroneCustomerSelectionProbability(candidateMembersIndex, depotIndex, launchVertexIndex, *subCluster);
            
            selectedMemberIndex = rouletteWheelSelection(
                selectionProbability,
                subCluster->size
            );
            
            droneCustomerIndex = subCluster->elements[selectedMemberIndex];
        }

    }

    free(candidateMembersIndex);

    return droneCustomerIndex;
}

void StodolaInspiredAntSystem::updateDroneCustomerSelectionProbability(bool* candidateMembersIndex, int depotIndex, int vertexIndex, const SubCluster& subCluster) {

    fillArray(selectionProbability, subCluster.size, 0.0);

    for(int memberIndex = 0; memberIndex < subCluster.size; memberIndex++) {
        
        if(candidateMembersIndex[memberIndex]) {

            int neighborCustomerIndex = subCluster.elements[memberIndex];

            double weightedPheromoneConcentration = weightedValue(
                pheromone.droneMatrices[depotIndex][vertexIndex][neighborCustomerIndex], 
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

bool StodolaInspiredAntSystem::hasAchievedTerminationCondition(int iterationsCount, int iterationsWithoutImprovementCount, double currentOptimizationTime, double informationEntropyCoef) {

    return (iterationsCount >= maxIterations) ||
        (iterationsWithoutImprovementCount >= maxIterationsWithoutImprovement) ||
        (currentOptimizationTime >= maxOptimizationTime) ||
        (informationEntropyCoef < minInformationEntropyCoef);
}

bool StodolaInspiredAntSystem::canUseWorseSolution(const Solution& generationBestSolution) {

    double bestSolutionProbability = generationBestSolution.fitness - bestSolution.fitness;
    bestSolutionProbability /= bestSolution.fitness;
    bestSolutionProbability /= temperature;
    bestSolutionProbability *= -1;
    bestSolutionProbability = exp(bestSolutionProbability);

    double generationBestSolutionProbability = 1 - bestSolutionProbability;
    double randomValue = ((double)rand() / RAND_MAX);

    return (randomValue <= generationBestSolutionProbability);
}

bool StodolaInspiredAntSystem::canDroneVisitCustomer(const Route& route, const Sortie& sortie, const Customer& customer, const Truck& truck, const Drone& drone) {

    bool willDroneCapacityBeExceeded = customer.demand > drone.capacity;
    if(willDroneCapacityBeExceeded) {
        return false;
    }

    bool willTruckCapacityBeExceeded = (route.currentLoad() + customer.demand) > truck.capacity;
    if(willTruckCapacityBeExceeded) {
        return false;
    }

    double droneDeliveryDuration = problemInstance.calculateDeliveryDuration(drone, sortie);
    bool willDroneEnduranceBeExceededByDroneDelivery = droneDeliveryDuration > drone.endurance;
    if(willDroneEnduranceBeExceededByDroneDelivery) {
        return false;
    }

    bool existsTruckMaxDuration = truck.routeMaxDuration > 0;
    bool willTruckMaxDurationBeExceeded = existsTruckMaxDuration && (route.currentDuration() + droneDeliveryDuration) > truck.routeMaxDuration;
    if(willTruckMaxDurationBeExceeded) {
        return false;
    }

    double customerDeliveryDuration = problemInstance.calculateMovementDuration(truck, sortie.launchVertexIndex, sortie.recoveryVertexIndex);
    int depotVertexIndex = problemInstance.getDepotVertexIndex(route.depotIndex);
    
    bool willDroneRecoveryBeAtDepot = (sortie.recoveryVertexIndex == depotVertexIndex);
    if(willDroneRecoveryBeAtDepot) {
        customerDeliveryDuration += truck.serviceTime;
    }
    
    bool willDroneEnduranceBeExceededByTruckDelivery = (customerDeliveryDuration + drone.launchTime + drone.recoveryTime) > drone.endurance;
    if(willDroneEnduranceBeExceededByTruckDelivery) {
        return false;
    }

    return true;
}
