#include "../../include/solution/Route.hpp"

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

void Route::initializeNextSubRoute() {

    int nextSubRouteIndex = size - 1;
    int subRouteMaxLength = problemInstance->customersCount;
    subRoutes[nextSubRouteIndex] = SubRoute(problemInstance, depotIndex, nextSubRouteIndex, subRouteMaxLength);
}

void Route::finalize() {

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
        
        int subRouteMaxLength = subRoutes[0].maxLength;
        initializeNextSubRoute();
    }
}

void Route::insert(int customerIndex) {

    subRoutes[size - 1].insert(customerIndex);
}

void Route::reset() {

    for(int subRouteIndex = 0; subRouteIndex < maxSize; subRouteIndex++) {
        subRoutes[subRouteIndex].reset();
    }

    initializeValues();
}

void Route::copy(const Route& routeToCopy) {

    if(routeToCopy.size > maxSize) {

        subRoutes = (SubRoute*) realloc(subRoutes, routeToCopy.size * sizeof(SubRoute));

        int subRouteMaxLength = subRoutes[0].maxLength;
        for(int subRouteIndex = maxSize; subRouteIndex < routeToCopy.size; subRouteIndex++) {
            subRoutes[subRouteIndex] = SubRoute(problemInstance, depotIndex, subRouteIndex, subRouteMaxLength);
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

void Route::incrementCurrentDuration(double deliveryDuration) {

    subRoutes[size - 1].incrementDuration(deliveryDuration);
}

void Route::incrementCurrentLoad(double demand) {

    subRoutes[size - 1].incrementLoad(demand);
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

int Route::last() const {

    return subRoutes[size - 1].last();
}

double Route::currentDuration() const {

    return subRoutes[size - 1].duration;
}

double Route::currentLoad() const {

    return subRoutes[size - 1].load;
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