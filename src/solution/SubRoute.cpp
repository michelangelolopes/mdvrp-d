#include "../../include/solution/SubRoute.hpp"

void SubRoute::init() {

    initializeValues();
    initializeMembers();
}

void SubRoute::initializeValues() {

    length = 0;
    load = 0;
    distanceTraveled = -1;
    timeSpent = -1;
}

void SubRoute::initializeMembers() {

    members = (int*) malloc(maxLength * sizeof(int));
}

void SubRoute::finalize() {

    free(members);
}

void SubRoute::insert(int customerIndex) {

    members[length++] = customerIndex;
}

void SubRoute::reset() {

    initializeValues();
}

void SubRoute::copy(SubRoute subRouteToCopy) {

    length = subRouteToCopy.length;
    load = subRouteToCopy.load;
    distanceTraveled = subRouteToCopy.distanceTraveled;
    timeSpent = subRouteToCopy.timeSpent;

    for(int memberIndex = 0; memberIndex < subRouteToCopy.length; memberIndex++) {
        members[memberIndex] = subRouteToCopy.members[memberIndex];
    }
}

void SubRoute::incrementLoad(double demand) {

    load += demand;
}

void SubRoute::updateDistanceTraveled(ProblemInstance problemInstance) {

    distanceTraveled = 0;

    for(int customerIndex = 0; customerIndex < length - 1; customerIndex++) {

        int sourceIndex = members[customerIndex];
        int destIndex = members[customerIndex + 1];
        distanceTraveled += problemInstance.verticesDistanceMatrix[sourceIndex][destIndex];
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
        return depotIndex + maxLength;
    }

    return members[0];
}

int SubRoute::last() const {

    if(length == 0) {
        return depotIndex + maxLength;
    }

    return members[length - 1];
}