#include "../../include/metaheuristics/Pheromone.hpp"

#include "../../include/utils/ArrayUtils.hpp"
#include "../../include/utils/MathUtils.hpp"

void Pheromone::init() {

    initializeTruckMatrices();
    initializeDroneMatrices();
}

void Pheromone::finalize() {

    if(truckMatrices != nullptr) {

        for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
            freeMatrix(truckMatrices[depotIndex], problemInstance->verticesCount);
        }

        free(truckMatrices);
    }

    if(droneMatrices != nullptr) {

        for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
            freeMatrix(droneMatrices[depotIndex], problemInstance->verticesCount);
        }

        free(droneMatrices);
    }
}

void Pheromone::updateReinforcementValue(const Solution& bestSolution, const Solution& consideredSolution) {
    reinforcementValue = reinforcementCoef * (bestSolution.fitness / consideredSolution.fitness);
}

void Pheromone::updateEvaporationValue(const InformationEntropy& informationEntropy) {

    double evaporationCoef = informationEntropy.normalized * (evaporationCoefMax - evaporationCoefMin) + evaporationCoefMin;

    evaporationValue = (1 - evaporationCoef);
}

void Pheromone::initializeTruckMatrices() {

    truckMatrices = (double***) malloc(problemInstance->depotsCount * sizeof(double**));

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
        truckMatrices[depotIndex] = (double**) mallocMatrix(problemInstance->verticesCount, sizeof(double*), sizeof(double));
        fillMatrix(truckMatrices[depotIndex], problemInstance->verticesCount, 1.0);
    }
}

void Pheromone::initializeDroneMatrices() {

    droneMatrices = (double***) malloc(problemInstance->depotsCount * sizeof(double**));

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
        //depotVertex is considered a possible destination 
        droneMatrices[depotIndex] = (double**) mallocMatrix(problemInstance->verticesCount, sizeof(double*), sizeof(double));
        fillMatrix(droneMatrices[depotIndex], problemInstance->verticesCount, 1.0);
    }
}

void Pheromone::evaporateTruckMatrices() {

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
        transformMatrix(truckMatrices[depotIndex], problemInstance->verticesCount, evaporationValue, multiply);
    }
}

void Pheromone::evaporateDroneMatrices() {

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
        transformMatrix(droneMatrices[depotIndex], problemInstance->verticesCount, evaporationValue, multiply);
    }
}

void Pheromone::reinforceTruckMatrices(const Solution& consideredSolution) {

    //obs: checking precedence is not needed, because only the pheromone of visitedCustomers from the route will be updated
    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
        
        int depotVertexIndex = problemInstance->getDepotVertexIndex(depotIndex);
        Route* route = &consideredSolution.routes[depotIndex];

        for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {
        
            SubRoute* subRoute = &route->subRoutes[subRouteIndex];

            int firstCustomerIndex = subRoute->first();
            int lastCustomerIndex = subRoute->last();

            truckMatrices[depotIndex][depotVertexIndex][firstCustomerIndex] += reinforcementValue;
            truckMatrices[depotIndex][depotVertexIndex][lastCustomerIndex] += reinforcementValue;

            for(int memberIndex = 0; memberIndex < subRoute->length - 1; memberIndex++) {

                int customerIndex = subRoute->members[memberIndex];
                int neighborCustomerIndex = subRoute->members[memberIndex + 1];
                truckMatrices[depotIndex][customerIndex][neighborCustomerIndex] += reinforcementValue;
            }
        }
    }
}

void Pheromone::reinforceDroneMatrices(const Solution& consideredSolution) {

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {

        DroneRoute* droneRoute = &consideredSolution.routes[depotIndex].droneRoute;

        for(int sortieIndex = 0; sortieIndex < droneRoute->size; sortieIndex++) {

            Sortie* sortie = &droneRoute->sorties[sortieIndex];

            droneMatrices[depotIndex][sortie->launchVertexIndex][sortie->deliveryVertexIndex] += reinforcementValue;
            droneMatrices[depotIndex][sortie->deliveryVertexIndex][sortie->recoveryVertexIndex] += reinforcementValue;
        }
    }
}
