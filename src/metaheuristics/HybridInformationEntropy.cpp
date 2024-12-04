#include "../../include/metaheuristics/HybridInformationEntropy.hpp"

#include <cmath>
#include "../../include/utils/ArrayUtils.hpp"

void HybridInformationEntropy::init() {

    if(truckEdgesOccurrence == nullptr) {
        truckEdgesOccurrence = (int**) mallocMatrix(problemInstance->verticesCount, sizeof(int*), sizeof(int));
    }

    if(droneEdgesOccurrence == nullptr) {
        droneEdgesOccurrence = (int**) mallocMatrix(problemInstance->verticesCount, sizeof(int*), sizeof(int));
    }
}

void HybridInformationEntropy::finalize() {

    InformationEntropy::finalize();

    if(droneEdgesOccurrence != nullptr) {
        freeMatrix(droneEdgesOccurrence, problemInstance->verticesCount);
    }
}

void HybridInformationEntropy::update() {

    int totalEdgesCount = truckEdgesCount + droneEdgesCount;

    cur = calculate();
    min = -1 * log2((double)antsCount / totalEdgesCount);
    max = -1 * log2(1.00 / totalEdgesCount);

    double offset = (cur - min);
    double range = (max - min);
    
    coef = offset / min;
    normalized = range <= 0 ? 1 : (offset / range);
}

void HybridInformationEntropy::resetEdgesValues() {

    InformationEntropy::resetEdgesValues();

    droneEdgesCount = 0;
    fillMatrix(droneEdgesOccurrence, problemInstance->verticesCount, 0);
}

void HybridInformationEntropy::updateEdgesOccurrence(const Solution& solution) {

    InformationEntropy::updateEdgesOccurrence(solution);

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {

        DroneRoute* droneRoute = &solution.routes[depotIndex].droneRoute;

        for(int sortieIndex = 0; sortieIndex < droneRoute->size; sortieIndex++) {

            Sortie* sortie = &droneRoute->sorties[sortieIndex];

            droneEdgesOccurrence[sortie->launchVertexIndex][sortie->deliveryVertexIndex] += 1;
            droneEdgesOccurrence[sortie->deliveryVertexIndex][sortie->recoveryVertexIndex] += 1;

            droneEdgesCount += 2;
        }
    }
}

double HybridInformationEntropy::calculate() {
    
    double entropy = 0;

    int totalGenerationEdgesCount = truckEdgesCount + droneEdgesCount;
    for(int vertexIndex = 0; vertexIndex < problemInstance->verticesCount; vertexIndex++) {
        for(int neighborVertexIndex = 0; neighborVertexIndex < problemInstance->verticesCount; neighborVertexIndex++) {

            double truckEdgeOccurrenceCount = truckEdgesOccurrence[vertexIndex][neighborVertexIndex];
            double droneEdgeOccurrenceCount = droneEdgesOccurrence[vertexIndex][neighborVertexIndex];

            int vehicleEdgeOccurrenceCount = truckEdgeOccurrenceCount + droneEdgeOccurrenceCount;
            if(vehicleEdgeOccurrenceCount > 0) {

                double edgeOccurrenceProbability = (double) vehicleEdgeOccurrenceCount / (double) totalGenerationEdgesCount;
                double balancedEdgeOccurrenceProbability = (edgeOccurrenceProbability * log2(edgeOccurrenceProbability));

                entropy += balancedEdgeOccurrenceProbability;
            }
        }
    }
    
    entropy *= -1;

    return entropy;
}