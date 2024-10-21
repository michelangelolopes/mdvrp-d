#include "../../include/solution/SubRoute.h"

void SubRoute::initialize() {

    members = (int*) calloc(maxLength, sizeof(int));
}

void SubRoute::finalize() {

    free(members);
}

void SubRoute::shrink() {

    if(length > 0) {
        members = (int*) realloc(members, length * sizeof(int));
    }
}

void SubRoute::insert(int customerIndex) {

    members[length++] = customerIndex;
}

void SubRoute::incrementLoad(double demand) {

    load += demand;
}

void SubRoute::updateDistanceTraveled(ProblemInstance problemInstance) {

    distanceTraveled = 0;

    for(int customerIndex = 0; customerIndex < length - 1; customerIndex++) {

        int sourceIndex = members[customerIndex];
        int destIndex = members[customerIndex + 1];
        distanceTraveled += problemInstance.customerToCustomerDistanceMatrix[sourceIndex][destIndex];
    }
}

void SubRoute::updateTimeSpent(ProblemInstance problemInstance, int depotIndex) {

    updateDistanceTraveled(problemInstance);
    double truckSpeed = problemInstance.depots[depotIndex].truck.speed;

    timeSpent = (distanceTraveled / truckSpeed);
}

void SubRoute::print() const {

    for(int index = 0; index < length; index++) {

        // std::cout << members[index] + 1;
        std::cout << members[index];

        if(index != length - 1) {
            std::cout << " ";
        }
    }
}

int SubRoute::first() const {

    if(length == 0) {
        return -1;
    }

    return members[0];
}

int SubRoute::last() const {

    if(length == 0) {
        return -1;
    }

    return members[length - 1];
}