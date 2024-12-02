#include "../../include/metaheuristics/HybridPheromone.hpp"

#include "../../include/utils/ArrayUtils.hpp"
#include "../../include/utils/MathUtils.hpp"

void HybridPheromone::init() {

    Pheromone::init();

    if(droneMatrices == nullptr) {
        droneMatrices = (double***) malloc(problemInstance->depotsCount * sizeof(double**));

        for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
            //depotVertex is considered a possible destination 
            droneMatrices[depotIndex] = (double**) mallocMatrix(problemInstance->verticesCount, sizeof(double*), sizeof(double));
            fillMatrix(droneMatrices[depotIndex], problemInstance->verticesCount, 1.0);
        }
    }
}

void HybridPheromone::finalize() {

    Pheromone::finalize();

    if(droneMatrices != nullptr) {

        for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
            freeMatrix(droneMatrices[depotIndex], problemInstance->verticesCount);
        }

        free(droneMatrices);
    }
}

void HybridPheromone::evaporateMatrices(const InformationEntropy& informationEntropy) {

    Pheromone::evaporateMatrices(informationEntropy);

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
        transformMatrix(droneMatrices[depotIndex], problemInstance->verticesCount, evaporationValue, multiply);
    }
}

void HybridPheromone::reinforceMatrices(const Solution& bestSolution, const Solution& consideredSolution) {

    Pheromone::reinforceMatrices(bestSolution, consideredSolution);

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {

        DroneRoute* droneRoute = &consideredSolution.routes[depotIndex].droneRoute;

        for(int sortieIndex = 0; sortieIndex < droneRoute->size; sortieIndex++) {

            Sortie* sortie = &droneRoute->sorties[sortieIndex];

            droneMatrices[depotIndex][sortie->launchVertexIndex][sortie->deliveryVertexIndex] += reinforcementValue;
            droneMatrices[depotIndex][sortie->deliveryVertexIndex][sortie->recoveryVertexIndex] += reinforcementValue;
        }
    }
}
