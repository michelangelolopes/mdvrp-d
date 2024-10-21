#include "../../include/solution/Route.h"

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

    subRoutes = (SubRoute*) calloc(size, sizeof(SubRoute));
    initializeNextSubRoute(subRouteMaxLength);
}

void Route::initializeNextSubRoute(int subRouteMaxLength) {

    int nextSubRouteIndex = size - 1;
    subRoutes[nextSubRouteIndex] = SubRoute(subRouteMaxLength);
}

void Route::finalize() {

    if(subRoutes != nullptr) {

        for(int index = 0; index < size; index++) {
            subRoutes[index].finalize();
        }

        free(subRoutes);
    }
}

void Route::expand() {

    size++;

    if(size > maxSize) {

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

    // this->routes = (Route*) calloc(this->depotsCount, sizeof(Route));

    if(size > maxSize) {
        maxSize = size;
    }

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {
        this->subRoutes[subRouteIndex].reset();
    }

    initializeValues();
}

void Route::copy(Route routeToCopy) {

    if(routeToCopy.maxSize > maxSize) {
        maxSize = routeToCopy.size;  
    }

    if(routeToCopy.size > size) {

        subRoutes = (SubRoute*) realloc(subRoutes, routeToCopy.size * sizeof(SubRoute));

        int subRouteMaxLength = subRoutes[0].maxLength;
        for(int subRouteIndex = size; subRouteIndex < routeToCopy.size; subRouteIndex++) {
            subRoutes[subRouteIndex] = SubRoute(subRouteMaxLength);
        }

        size = routeToCopy.size;
    }

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

void Route::updateDistanceTraveled(ProblemInstance problemInstance, int depotIndex) {
    
    distanceTraveled = 0;

    for(int subRouteIndex = 0; subRouteIndex < size; subRouteIndex++) {

        int firstCustomerIndex = subRoutes[subRouteIndex].members[0];
        distanceTraveled += problemInstance.depotToCustomerDistanceMatrix[depotIndex][firstCustomerIndex];

        distanceTraveled += subRoutes[subRouteIndex].distanceTraveled;
        
        int lastCustomerIndex = subRoutes[subRouteIndex].last();
        distanceTraveled += problemInstance.depotToCustomerDistanceMatrix[depotIndex][lastCustomerIndex]; //undirected graph -> same distance
    }
}

void Route::updateTimeSpent(ProblemInstance problemInstance, int depotIndex) {

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