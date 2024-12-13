#include "../../include/metaheuristics/HybridAntSystem.hpp"

#include <chrono>

#include "../../include/utils/ArrayUtils.hpp"
#include "../../include/utils/MathUtils.hpp"

void HybridAntSystem::run() {

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

        // //TODO: local optimization inter-route exchange
        // if(iterationsCount % localOptimizationFrequency == 0) {
        //     localOptimization(generationBestSolution);
        // }

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
    
    bestSolution.printWithDrone();

    std::cout << "checkConstraints: " << bestSolution.checkConstraints() << std::endl;
}

void HybridAntSystem::buildAntRoutes(Solution& antSolution) {

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
        // antSolution.printWithDrone(depotIndex);
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

        bool willTruckExceedMaxDuration = false;
        double customerDeliveryDuration = problemInstance.calculateDeliveryDuration(*currentTruck, currentVertexIndex, customerIndex);
        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);

        if(currentTruck->routeMaxDuration > 0) {

            double depotReturnDuration = problemInstance.calculateMovementDuration(*currentTruck, customerIndex, depotVertexIndex);
            double updatedTruckDuration = (currentSubRoute->duration + customerDeliveryDuration + depotReturnDuration);
            
            willTruckExceedMaxDuration = updatedTruckDuration > currentTruck->routeMaxDuration;
        }

        if(willTruckExceedCapacity || willTruckExceedMaxDuration) {

            double depotReturnDuration = problemInstance.calculateMovementDuration(*currentTruck, currentVertexIndex, depotVertexIndex);
            bool canDroneDeliver = (unvisitedCustomersCount > 0);
            bool hasDroneDelivered = false;
            if(canDroneDeliver) {

                int droneSubClusterIndex = selectSubCluster(depotIndex, currentVertexIndex, pheromone.droneMatrices);
                // std::cout << "------------ droneSubClusterIndex: " << droneSubClusterIndex << "\n";
                int droneCustomerIndex = selectDroneCustomer(depotIndex, droneSubClusterIndex, currentVertexIndex, depotVertexIndex, *currentSubRoute);
                // std::cout << "------------ droneCustomerIndex: " << droneCustomerIndex << "\n";

                hasDroneDelivered = (droneCustomerIndex != -1);
                if(hasDroneDelivered) {
                    DroneRoute* currentDroneRoute = &antSolution.routes[depotIndex].droneRoute;
                    Customer* nextDroneCustomer = &problemInstance.customers[droneCustomerIndex];
                    Drone* currentDrone = &problemInstance.depots[depotIndex].drone;

                    Sortie sortie(problemInstance, currentVertexIndex, droneCustomerIndex, depotVertexIndex);
                    sortie.duration = problemInstance.calculateDeliveryDuration(*currentDrone, sortie.launchVertexIndex, sortie.deliveryVertexIndex, sortie.recoveryVertexIndex);
                    sortie.subRouteIndex = currentSubRoute->subRouteIndex;

                    // sortie.print();
                    // cout << endl;                    

                    double truckFullDuration = depotReturnDuration + currentDrone->launchTime + currentDrone->recoveryTime;
                    double vehicleLongestDuration = std::max(truckFullDuration, sortie.duration);

                    currentDroneRoute->insert(sortie);
                    currentSubRoute->incrementLoad(nextDroneCustomer->demand);
                    currentSubRoute->incrementDuration(vehicleLongestDuration);

                    visitedCustomersIndexes[droneCustomerIndex] = 1;
                    unvisitedCustomersCount--;
                }
            }

            if(!canDroneDeliver || !hasDroneDelivered) {
                currentSubRoute->incrementDuration(depotReturnDuration);
            }

            currentRoute->expand();
            currentSubRoute = &currentRoute->last();
            currentVertexIndex = depotVertexIndex;
        }

        currentSubRoute->insert(customerIndex);
        currentSubRoute->incrementLoad(nextCustomer->demand);

        visitedCustomersIndexes[customerIndex] = 1;
        unvisitedCustomersCount--;

        bool canDroneDeliver = (unvisitedCustomersCount > 0);
        bool hasDroneDelivered = false;
        customerDeliveryDuration = problemInstance.calculateDeliveryDuration(*currentTruck, currentVertexIndex, customerIndex);
        if(canDroneDeliver) {

            int droneSubClusterIndex = selectSubCluster(depotIndex, currentVertexIndex, pheromone.droneMatrices);
            // std::cout << "------------ droneSubClusterIndex: " << droneSubClusterIndex << "\n";
            int droneCustomerIndex = selectDroneCustomer(depotIndex, droneSubClusterIndex, currentVertexIndex, customerIndex, *currentSubRoute);
            // std::cout << "------------ droneCustomerIndex: " << droneCustomerIndex << "\n";
            hasDroneDelivered = (droneCustomerIndex != -1);
            if(hasDroneDelivered) {
                DroneRoute* currentDroneRoute = &antSolution.routes[depotIndex].droneRoute;
                Customer* nextDroneCustomer = &problemInstance.customers[droneCustomerIndex];
                Drone* currentDrone = &problemInstance.depots[depotIndex].drone;

                Sortie sortie(problemInstance, currentVertexIndex, droneCustomerIndex, customerIndex);
                sortie.duration = problemInstance.calculateDeliveryDuration(*currentDrone, sortie.launchVertexIndex, sortie.deliveryVertexIndex, sortie.recoveryVertexIndex);
                sortie.subRouteIndex = currentSubRoute->subRouteIndex;

                // sortie.print();
                // cout << endl;
                
                double truckFullDuration = customerDeliveryDuration + currentDrone->launchTime + currentDrone->recoveryTime;
                double vehicleLongestDuration = std::max(truckFullDuration, sortie.duration);
                
                currentDroneRoute->insert(sortie);
                currentSubRoute->incrementLoad(nextDroneCustomer->demand);
                currentSubRoute->incrementDuration(vehicleLongestDuration);

                visitedCustomersIndexes[droneCustomerIndex] = 1;
                unvisitedCustomersCount--;
            }
        }

        if(!canDroneDeliver || !hasDroneDelivered) {
            currentSubRoute->incrementDuration(customerDeliveryDuration);
            currentRoute->duration = currentRoute->calculateDuration();
        }
    }

    for(int depotIndex = 0; depotIndex < problemInstance.depotsCount; depotIndex++) {
        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);
        SubRoute* currentSubRoute = &antSolution.routes[depotIndex].last();
        Truck* currentTruck = &problemInstance.depots[depotIndex].truck;
        double depotReturnDuration = problemInstance.calculateMovementDuration(*currentTruck, currentSubRoute->last(), depotVertexIndex);
        currentSubRoute->incrementDuration(depotReturnDuration);
    }


    antSolution.updateFitnessWithDrone();
}

int HybridAntSystem::selectDroneCustomer(int depotIndex, int droneSubClusterIndex, int launchVertexIndex, int recoveryVertexIndex, const SubRoute& subRoute) {

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

            Sortie sortie(problemInstance, launchVertexIndex, neighborCustomerIndex, recoveryVertexIndex);
            Customer* droneCustomer = &problemInstance.customers[neighborCustomerIndex];
            bool canDroneVisitCustomer = sortie.isFeasible(subRoute, neighborCustomerIndex);
            if(canDroneVisitCustomer) {
                candidateMembersIndex[memberIndex] = true;
                candidatesCount++;

                // std::cout << "pheromoneConcentration: " << pheromone.droneMatrices[depotIndex][launchVertexIndex][neighborCustomerIndex] << "\n";
                maxPheromoneConcentration = std::max(
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

void HybridAntSystem::updateDroneCustomerSelectionProbability(bool* candidateMembersIndex, int depotIndex, int vertexIndex, const SubCluster& subCluster) {

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
