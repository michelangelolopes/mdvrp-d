#include "../../include/solution/Route.h"

void Route::initialize(int subRouteMaxLength) {

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

    subRoutes = (SubRoute*) realloc(subRoutes, ++size * sizeof(SubRoute));

    int subRouteMaxLength = subRoutes[0].maxLength;
    initializeNextSubRoute(subRouteMaxLength);
}

void Route::shrink() {

    for(int index = 0; index < size; index++) {
        subRoutes[index].shrink();
    }
}

void Route::insert(int customerIndex) {

    subRoutes[size - 1].insert(customerIndex);
}

int Route::last() {

    return subRoutes[size - 1].last();
}

double Route::getCurrentLoad() {

    return subRoutes[size - 1].load;
}

void Route::updateCurrentLoad(double demand) {

    subRoutes[size - 1].updateLoad(demand);
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