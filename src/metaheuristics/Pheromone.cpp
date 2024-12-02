#include "../../include/metaheuristics/Pheromone.hpp"

#include "../../include/utils/ArrayUtils.hpp"
#include "../../include/utils/MathUtils.hpp"

void Pheromone::init() {

    if(truckMatrices == nullptr) {
        truckMatrices = (double***) malloc(problemInstance->depotsCount * sizeof(double**));

        for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
            truckMatrices[depotIndex] = (double**) mallocMatrix(problemInstance->verticesCount, sizeof(double*), sizeof(double));
            fillMatrix(truckMatrices[depotIndex], problemInstance->verticesCount, 1.0);
        }
    }
}

void Pheromone::finalize() {

    if(truckMatrices != nullptr) {

        for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
            freeMatrix(truckMatrices[depotIndex], problemInstance->verticesCount);
        }

        free(truckMatrices);
    }
}

void Pheromone::evaporateMatrices(const InformationEntropy& informationEntropy) {

    updateEvaporationValue(informationEntropy);

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
        transformMatrix(truckMatrices[depotIndex], problemInstance->verticesCount, evaporationValue, multiply);
    }
}

void Pheromone::reinforceMatrices(const Solution& bestSolution, const Solution& consideredSolution) {

    updateReinforcementValue(bestSolution, consideredSolution);

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

void Pheromone::updateEvaporationValue(const InformationEntropy& informationEntropy) {

    double evaporationCoef = informationEntropy.normalized * (evaporationCoefMax - evaporationCoefMin) + evaporationCoefMin;

    evaporationValue = (1 - evaporationCoef);
}

void Pheromone::updateReinforcementValue(const Solution& bestSolution, const Solution& consideredSolution) {
    reinforcementValue = reinforcementCoef * (bestSolution.fitness / consideredSolution.fitness);
}