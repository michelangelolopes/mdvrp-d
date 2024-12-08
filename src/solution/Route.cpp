#include "../../include/solution/Route.hpp"

#include "../../include/utils/ArrayUtils.hpp"

void Route::init() {

    maxSize = 1;
    initializeValues();
    initializeSubRoutes();
}

void Route::initializeValues() {

    size = 1;
    distanceTraveled = -1;
    timeSpent = -1;
}

void Route::initializeSubRoutes() {

    subRoutes = (SubRoute*) malloc(size * sizeof(SubRoute));
    initializeNextSubRoute();
}

void Route::initializeNextSubRoute() {

    int nextSubRouteIndex = size - 1;
    subRoutes[nextSubRouteIndex] = SubRoute(problemInstance, depotIndex, nextSubRouteIndex);
}

void Route::finalize() {

    droneRoute.finalize();

    if(subRoutes != nullptr) {

        for(int index = 0; index < maxSize; index++) {
            subRoutes[index].finalize();
        }

        free(subRoutes);
    }
}

void Route::expand() {

    size++;

    if(size > maxSize) { //realloc only if its is not allocatted yet
        
        maxSize = size;
        subRoutes = (SubRoute*) realloc(subRoutes, size * sizeof(SubRoute));

        initializeNextSubRoute();
    }
}

void Route::reset() {

    droneRoute.reset();

    for(int subRouteIndex = 0; subRouteIndex < maxSize; subRouteIndex++) {
        subRoutes[subRouteIndex].resetValues();
    }

    initializeValues();
}

void Route::copy(const Route& routeToCopy) {

    if(routeToCopy.size > maxSize) {

        subRoutes = (SubRoute*) realloc(subRoutes, routeToCopy.size * sizeof(SubRoute));

        for(int subRouteIndex = maxSize; subRouteIndex < routeToCopy.size; subRouteIndex++) {
            subRoutes[subRouteIndex] = SubRoute(problemInstance, depotIndex, subRouteIndex);
        }

        maxSize = routeToCopy.size;
    }

    size = routeToCopy.size;
    distanceTraveled = routeToCopy.distanceTraveled;
    timeSpent = routeToCopy.timeSpent;

    for(int subRouteIndex = 0; subRouteIndex < routeToCopy.size; subRouteIndex++) {
        subRoutes[subRouteIndex].copy(routeToCopy.subRoutes[subRouteIndex]);
    }
}

void Route::shiftLeftSubRoutes(int subRouteOriginIndex) {

    SubRoute removedSubRoute = subRoutes[subRouteOriginIndex];
    shiftLeftArray(subRoutes, size, subRouteOriginIndex, 1);
    size--;

    subRoutes[size] = removedSubRoute;

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {
        subRoutes[subRouteIndex].subRouteIndex = subRouteIndex;
    }

}

void Route::shiftRightSubRoutes(int subRouteOriginIndex) {

    SubRoute removedSubRoute = subRoutes[size];
    shiftRightArray(subRoutes, size, subRouteOriginIndex, 1);
    size++;

    subRoutes[removedSubRoute.subRouteIndex] = removedSubRoute;

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {
        subRoutes[subRouteIndex].subRouteIndex = subRouteIndex;
    }

}

void Route::updateDistanceTraveled(int depotIndex) {
    
    distanceTraveled = 0;

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {

        int depotVertexIndex = problemInstance->getDepotVertexIndex(depotIndex);

        int firstCustomerIndex = subRoutes[subRouteIndex].first();
        int lastCustomerIndex = subRoutes[subRouteIndex].last();

        distanceTraveled += problemInstance->verticesDistanceMatrix[depotVertexIndex][firstCustomerIndex];
        distanceTraveled += problemInstance->verticesDistanceMatrix[depotVertexIndex][lastCustomerIndex]; //undirected graph -> same distance
        distanceTraveled += subRoutes[subRouteIndex].distanceTraveled;
        
    }
}

void Route::updateTimeSpent(int depotIndex) {

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {

        subRoutes[subRouteIndex].updateTimeSpent(depotIndex);
    }

    updateDistanceTraveled(depotIndex);

    double truckSpeed = problemInstance->depots[depotIndex].truck.speed;
    timeSpent = (distanceTraveled / truckSpeed);
}

double Route::calculateDuration() {

    int depotVertexIndex = problemInstance->getDepotVertexIndex(depotIndex);

    Truck* truck = &problemInstance->depots[depotIndex].truck;
    Drone* drone = &problemInstance->depots[depotIndex].drone;

    double duration = 0;
    bool* hasConsideredSortie = (bool*) calloc(droneRoute.size, sizeof(bool));

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {

        SubRoute* subRoute = &subRoutes[subRouteIndex];

        int firstCustomerIndex = subRoute->first();
        int lastCustomerIndex = subRoute->last();

        duration += getComposedDeliveryDuration(*truck, *drone, hasConsideredSortie, subRouteIndex, depotVertexIndex, firstCustomerIndex);
        duration += getComposedMovementDuration(*truck, *drone, hasConsideredSortie, subRouteIndex, lastCustomerIndex, depotVertexIndex);

        for(int memberIndex = 0; memberIndex < subRoute->length - 1; memberIndex++) {

            int customerIndex = subRoute->members[memberIndex];
            int neighborCustomerIndex = subRoute->members[memberIndex + 1];
            duration += getComposedDeliveryDuration(*truck, *drone, hasConsideredSortie, subRouteIndex, customerIndex, neighborCustomerIndex);
        }
    }

    free(hasConsideredSortie);

    return duration;
}

SubRoute& Route::last() const {
    return subRoutes[size - 1];
}

void Route::print() const {

    for(int index = 0; index < size; index++) {
        
        std::cout << "# ";
        subRoutes[index].print();

        if(index != size - 1) {
            std::cout << " ";
        }
    }

    std::cout << " #";
}

void Route::printWithDrone() const {

    int sortieIndex = 0;
    Sortie* sortie = &droneRoute.sorties[sortieIndex];
    int depotVertexIndex = problemInstance->getDepotVertexIndex(depotIndex);
    int customersInRouteCount = 0;
    bool hasDroneRouteEnded = (sortieIndex >= droneRoute.size);

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {
        
        std::cout << "# ";
        SubRoute* subRoute = &subRoutes[subRouteIndex];
        customersInRouteCount += subRoute->length;

        if(!hasDroneRouteEnded && depotVertexIndex == sortie->launchVertexIndex && subRoute->first() == sortie->recoveryVertexIndex) {
            // std::cout << endl;
            // std::cout << "depotVertexIndex: " << depotVertexIndex << endl;
            // std::cout << "launchVertexIndex: " << sortie->launchVertexIndex << endl;
            // std::cout << "subRoute->first(): " << subRoute->first() << endl;
            // std::cout << "recoveryVertexIndex: " << sortie->recoveryVertexIndex << endl;
            std::cout << "D" << sortie->deliveryVertexIndex;
            std::cout << " ";
            sortieIndex++;

            hasDroneRouteEnded = (sortieIndex >= droneRoute.size);
            if(!hasDroneRouteEnded) {
                sortie = &droneRoute.sorties[sortieIndex];
            }
        }

        for(int memberIndex = 0; memberIndex < subRoute->length; memberIndex++) {
            
            int customerIndex = subRoute->members[memberIndex];
            std::cout << customerIndex;

            if(!hasDroneRouteEnded && customerIndex == sortie->launchVertexIndex) {

                // std::cout << endl;
                // std::cout << "customerIndex: " << customerIndex << endl;
                // std::cout << "launchVertexIndex: " << sortie->launchVertexIndex << endl;
                // std::cout << "sortieIndex: " << sortieIndex << endl;
                // std::cout << "droneRoute.size: " << droneRoute.size << endl;

                std::cout << " ";
                std::cout << "D" << sortie->deliveryVertexIndex;
                sortieIndex++;
                
                hasDroneRouteEnded = (sortieIndex >= droneRoute.size);
                if(!hasDroneRouteEnded) {
                    sortie = &droneRoute.sorties[sortieIndex];
                }
            }

            if(memberIndex != subRoute->length - 1) {
                std::cout << " ";
            }
        }

        if(subRouteIndex != size - 1) {
            std::cout << " ";
        }
    }

    std::cout << " #";
    std::cout << " - (" << customersInRouteCount;
    std::cout << ", " << droneRoute.size << ")";
}

double Route::getComposedDuration(const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex, double truckDuration) {

    Sortie* sortie = checkSortieVertices(hasConsideredSortie, subRouteIndex, sourceIndex, destIndex);
    if(sortie == nullptr) {
        return truckDuration;
    }

    truckDuration += drone.launchTime + drone.recoveryTime;
    return std::max(truckDuration, sortie->duration);
}

double Route::getComposedDeliveryDuration(const Truck& truck, const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex) {

    double truckDuration = problemInstance->calculateDeliveryDuration(truck, sourceIndex, destIndex);

    return getComposedDuration(drone, hasConsideredSortie, subRouteIndex, sourceIndex, destIndex, truckDuration);
}

double Route::getComposedMovementDuration(const Truck& truck, const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex) {

    double truckDuration = problemInstance->calculateMovementDuration(truck, sourceIndex, destIndex);

    return getComposedDuration(drone, hasConsideredSortie, subRouteIndex, sourceIndex, destIndex, truckDuration);
}

Sortie* Route::checkSortieVertices(bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex) {

    Sortie* sortie = nullptr;
    for(int sortieIndex = 0; sortieIndex < droneRoute.size; sortieIndex++) {

        if(hasConsideredSortie[sortieIndex]) {
           continue; 
        }

        sortie = &droneRoute.sorties[sortieIndex];
        if(sortie->subRouteIndex != subRouteIndex) {
            continue;
        }

        if(sortie->isSameVertices(sourceIndex, destIndex)) {
            hasConsideredSortie[sortieIndex] = true;
            return sortie;
        }
    }

    return nullptr;
}
