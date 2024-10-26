#include "../../include/solution/Route.hpp"

void Route::init(int subRouteMaxLength) {

    maxSize = 1;
    initializeValues();
    initializeSubRoutes(subRouteMaxLength);
}

void Route::initializeValues() {

    size = 1;
    distanceTraveled = -1;
    timeSpent = -1;
}

void Route::initializeSubRoutes(int subRouteMaxLength) {

    subRoutes = (SubRoute*) malloc(size * sizeof(SubRoute));
    initializeNextSubRoute(subRouteMaxLength);
}

void Route::initializeNextSubRoute(int subRouteMaxLength) {

    int nextSubRouteIndex = size - 1;
    subRoutes[nextSubRouteIndex] = SubRoute(depotIndex, subRouteMaxLength);
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
        initializeNextSubRoute(subRouteMaxLength);
    }
}

void Route::insert(int customerIndex) {

    subRoutes[size - 1].insert(customerIndex);
}

int Route::last() {

    return subRoutes[size - 1].last();
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
            subRoutes[subRouteIndex] = SubRoute(depotIndex, subRouteMaxLength);
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

double Route::getCurrentLoad() {

    return subRoutes[size - 1].load;
}

void Route::incrementCurrentLoad(double demand) {

    subRoutes[size - 1].incrementLoad(demand);
}

void Route::updateDistanceTraveled(const ProblemInstance& problemInstance, int depotIndex) {
    
    distanceTraveled = 0;

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {

        int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);

        int firstCustomerIndex = subRoutes[subRouteIndex].first();
        int lastCustomerIndex = subRoutes[subRouteIndex].last();

        distanceTraveled += problemInstance.verticesDistanceMatrix[depotVertexIndex][firstCustomerIndex];
        distanceTraveled += problemInstance.verticesDistanceMatrix[depotVertexIndex][lastCustomerIndex]; //undirected graph -> same distance
        distanceTraveled += subRoutes[subRouteIndex].distanceTraveled;
        
    }
}

void Route::updateTimeSpent(const ProblemInstance& problemInstance, int depotIndex) {

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {

        subRoutes[subRouteIndex].updateTimeSpent(problemInstance, depotIndex);
    }

    updateDistanceTraveled(problemInstance, depotIndex);

    double truckSpeed = problemInstance.depots[depotIndex].truck.speed;
    timeSpent = (distanceTraveled / truckSpeed);
}

void Route::print() const {

    for(int index = 0; index < size; index++) {
        
        std::cout << "D ";
        subRoutes[index].print();

        if(index != size - 1) {
            std::cout << " ";
        }
    }

    std::cout << " D";
}