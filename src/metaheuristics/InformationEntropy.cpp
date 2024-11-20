#include "../../include/metaheuristics/InformationEntropy.hpp"

#include <cmath>
#include "../../include/utils/ArrayUtils.hpp"

void InformationEntropy::init() {

    truckEdgesOccurrence = (int**) mallocMatrix(problemInstance->verticesCount, sizeof(int*), sizeof(int));
    droneEdgesOccurrence = (int**) mallocMatrix(problemInstance->verticesCount, sizeof(int*), sizeof(int));
}

void InformationEntropy::finalize() {

    freeMatrix(truckEdgesOccurrence, problemInstance->verticesCount);
    freeMatrix(droneEdgesOccurrence, problemInstance->verticesCount);
}

void InformationEntropy::update() {

    cur = calculate();
    min = -1 * log2((double)antsCount / truckEdgesCount);
    max = -1 * log2(1.00 / truckEdgesCount);

    double offset = (cur - min);
    double range = (max - min);

    coef = offset / min;
    normalized = range <= 0 ? 1 : (offset / range);
}

void InformationEntropy::updateWithDrone() {

    int totalEdgesCount = truckEdgesCount + droneEdgesCount;

    cur = calculateWithDrone();
    min = -1 * log2((double)antsCount / totalEdgesCount);
    max = -1 * log2(1.00 / totalEdgesCount);

    double offset = (cur - min);
    double range = (max - min);
    
    coef = offset / min;
    normalized = range <= 0 ? 1 : (offset / range);
}

void InformationEntropy::updateTruckEdgesOccurrence(const Solution& solution) {

    //TODO: it could be needed to use the depot-customer edges

    for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
        
        int depotVertexIndex = problemInstance->getDepotVertexIndex(depotIndex);
        Route* route = &solution.routes[depotIndex];

        for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {

            SubRoute* subRoute = &route->subRoutes[subRouteIndex];
            
            int firstCustomerIndex = subRoute->first();
            int lastCustomerIndex = subRoute->last();

            truckEdgesOccurrence[depotVertexIndex][firstCustomerIndex] += 1;
            truckEdgesOccurrence[lastCustomerIndex][depotVertexIndex] += 1;

            truckEdgesCount += 2;

            for(int memberIndex = 0; memberIndex < subRoute->length - 1; memberIndex++) {
                int customerIndex = subRoute->members[memberIndex];
                int neighborCustomerIndex = subRoute->members[memberIndex + 1];

                truckEdgesOccurrence[customerIndex][neighborCustomerIndex] += 1;
                truckEdgesCount += 1;
            }
        }
    }
}

void InformationEntropy::updateDroneEdgesOccurrence(const Solution& solution) {

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

void InformationEntropy::resetTruckEdgesValues() {
    truckEdgesCount = 0;
    fillMatrix(truckEdgesOccurrence, problemInstance->verticesCount, 0);
}

void InformationEntropy::resetDroneEdgesValues() {
    droneEdgesCount = 0;
    fillMatrix(droneEdgesOccurrence, problemInstance->verticesCount, 0);
}

double InformationEntropy::calculate() {
    
    double entropy = 0;

    for(int vertexIndex = 0; vertexIndex < problemInstance->verticesCount; vertexIndex++) {
        for(int neighborVertexIndex = 0; neighborVertexIndex < problemInstance->verticesCount; neighborVertexIndex++) {

            double edgeOccurrenceCount = truckEdgesOccurrence[vertexIndex][neighborVertexIndex];

            if(edgeOccurrenceCount > 0) {

                // std::cout << "(" << vertexIndex << ", " << neighborVertexIndex << "): ";
                // std::cout << edgeOccurrenceCount << " - ";
                // std::cout << generationEdgesCount << " - ";

                double edgeOccurrenceProbability = edgeOccurrenceCount / truckEdgesCount;
                // std::cout << edgeOccurrenceProbability << " - ";

                double balancedEdgeOccurrenceProbability = (edgeOccurrenceProbability * log2(edgeOccurrenceProbability));
                // std::cout << balancedEdgeOccurrenceProbability << "\n";

                entropy += balancedEdgeOccurrenceProbability;
            }
        }
    }
    
    entropy *= -1;

    return entropy;
}

double InformationEntropy::calculateWithDrone() {
    
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