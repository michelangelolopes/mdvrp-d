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

void SubRoute::copy(const SubRoute& subRouteToCopy) {

    length = subRouteToCopy.length;
    load = subRouteToCopy.load;
    distanceTraveled = subRouteToCopy.distanceTraveled;
    timeSpent = subRouteToCopy.timeSpent;

    for(int memberIndex = 0; memberIndex < subRouteToCopy.length; memberIndex++) {
        members[memberIndex] = subRouteToCopy.members[memberIndex];
    }
}

int SubRoute::constraints(const ProblemInstance& problemInstance) {

    return load <= problemInstance.depots[depotIndex].truck.capacity;
}

void SubRoute::incrementLoad(double demand) {

    load += demand;
}

void SubRoute::updateDistanceTraveled(const ProblemInstance& problemInstance) {

    distanceTraveled = 0;

    for(int customerIndex = 0; customerIndex < length - 1; customerIndex++) {

        int sourceIndex = members[customerIndex];
        int destIndex = members[customerIndex + 1];
        distanceTraveled += problemInstance.verticesDistanceMatrix[sourceIndex][destIndex];
    }
}

void SubRoute::updateTimeSpent(const ProblemInstance& problemInstance, int depotIndex) {

    updateDistanceTraveled(problemInstance);
    double truckSpeed = problemInstance.depots[depotIndex].truck.speed;

    timeSpent = (distanceTraveled / truckSpeed);
}

void SubRoute::exchangeMembers(const ProblemInstance& problemInstance, int memberIndexA, int memberIndexB, int successiveVerticesCount) {

    for(int successiveIndex = 0; successiveIndex < successiveVerticesCount; successiveIndex++) {

        int successiveMemberIndexA = memberIndexA + successiveIndex;
        int successiveMemberIndexB = memberIndexB + successiveIndex;

        int customerIndexA = members[successiveMemberIndexA];
        int customerIndexB = members[successiveMemberIndexB];

        Customer* customerA = &problemInstance.customers[customerIndexA];
        Customer* customerB = &problemInstance.customers[customerIndexB];

        // std::cout << "exchange:successiveIndex: " << successiveIndex << "\n";
        // std::cout << "exchange:successiveMemberIndexA: " << successiveMemberIndexA << " - ";
        // std::cout << "exchange:customerIndexA: " << customerIndexA << " - ";
        // std::cout << "exchange:customerA->demand: " << customerA->demand << "\n";
        // std::cout << "exchange:successiveMemberIndexB: " << successiveMemberIndexB << " - ";
        // std::cout << "exchange:customerIndexB: " << customerIndexB << " - ";
        // std::cout << "exchange:customerB->demand: " << customerB->demand << "\n";

        swap(members[successiveMemberIndexA], members[successiveMemberIndexB]);
    }
}

void SubRoute::revertExchangeMembers(const ProblemInstance& problemInstance, int memberIndexA, int memberIndexB, int successiveVerticesCount) {

    for(int successiveIndex = successiveVerticesCount - 1; successiveIndex >= 0; successiveIndex--) {

        int successiveMemberIndexA = memberIndexA + successiveIndex;
        int successiveMemberIndexB = memberIndexB + successiveIndex;

        int customerIndexA = members[successiveMemberIndexA];
        int customerIndexB = members[successiveMemberIndexB];

        Customer* customerA = &problemInstance.customers[customerIndexA];
        Customer* customerB = &problemInstance.customers[customerIndexB];

        // std::cout << "revert:successiveIndex: " << successiveIndex << "\n";
        // std::cout << "revert:successiveMemberIndexA: " << successiveMemberIndexA << " - ";
        // std::cout << "revert:customerIndexA: " << customerIndexA << " - ";
        // std::cout << "revert:customerA->demand: " << customerA->demand << "\n";
        // std::cout << "revert:successiveMemberIndexB: " << successiveMemberIndexB << " - ";
        // std::cout << "revert:customerIndexB: " << customerIndexB << " - ";
        // std::cout << "revert:customerB->demand: " << customerB->demand << "\n";

        swap(members[successiveMemberIndexA], members[successiveMemberIndexB]);
    }
}

void SubRoute::print() const {

    for(int index = 0; index < length; index++) {

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

void exchangeMembersBetweenSubRoutes(
    const ProblemInstance& problemInstance,
    SubRoute& subRouteA,
    SubRoute& subRouteB,
    int memberIndexA,
    int memberIndexB,
    int successiveVerticesCount
) {

    for(int successiveIndex = 0; successiveIndex < successiveVerticesCount; successiveIndex++) {

        int subRouteAMemberIndex = memberIndexA + successiveIndex;
        int subRouteBMemberIndex = memberIndexB + successiveIndex;

        int subRouteACustomerIndex = subRouteA.members[subRouteAMemberIndex];
        int subRouteBCustomerIndex = subRouteB.members[subRouteBMemberIndex];

        Customer* subRouteACustomer = &problemInstance.customers[subRouteACustomerIndex];
        Customer* subRouteBCustomer = &problemInstance.customers[subRouteBCustomerIndex];

        subRouteA.load += (subRouteBCustomer->demand - subRouteACustomer->demand);
        subRouteB.load += (subRouteACustomer->demand - subRouteBCustomer->demand);

        // std::cout << "successiveIndex: " << successiveIndex << "\n";
        // std::cout << "subRouteAMemberIndex: " << subRouteAMemberIndex << " - ";
        // std::cout << "subRouteACustomerIndex: " << subRouteACustomerIndex << " - ";
        // std::cout << "subRouteACustomer->demand: " << subRouteACustomer->demand << "\n";
        // std::cout << "subRouteBMemberIndex: " << subRouteBMemberIndex << " - ";
        // std::cout << "subRouteBCustomerIndex: " << subRouteBCustomerIndex << " - ";
        // std::cout << "subRouteBCustomer->demand: " << subRouteBCustomer->demand << "\n";

        swap(subRouteA.members[subRouteAMemberIndex], subRouteB.members[subRouteBMemberIndex]);
    }
}

void revertExchangeMembersBetweenSubRoutes(
    const ProblemInstance& problemInstance,
    SubRoute& subRouteA,
    SubRoute& subRouteB,
    int memberIndexA,
    int memberIndexB,
    int successiveVerticesCount
) {

    for(int successiveIndex = successiveVerticesCount - 1; successiveIndex >= 0; successiveIndex--) {

        int subRouteAMemberIndex = memberIndexA + successiveIndex;
        int subRouteBMemberIndex = memberIndexB + successiveIndex;

        int subRouteACustomerIndex = subRouteA.members[subRouteAMemberIndex];
        int subRouteBCustomerIndex = subRouteB.members[subRouteBMemberIndex];

        Customer* subRouteACustomer = &problemInstance.customers[subRouteACustomerIndex];
        Customer* subRouteBCustomer = &problemInstance.customers[subRouteBCustomerIndex];

        subRouteA.load += (subRouteBCustomer->demand - subRouteACustomer->demand);
        subRouteB.load += (subRouteACustomer->demand - subRouteBCustomer->demand);

        // std::cout << "successiveIndex: " << successiveIndex << "\n";
        // std::cout << "subRouteAMemberIndex: " << subRouteAMemberIndex << " - ";
        // std::cout << "subRouteACustomerIndex: " << subRouteACustomerIndex << " - ";
        // std::cout << "subRouteACustomer->demand: " << subRouteACustomer->demand << "\n";
        // std::cout << "subRouteBMemberIndex: " << subRouteBMemberIndex << " - ";
        // std::cout << "subRouteBCustomerIndex: " << subRouteBCustomerIndex << " - ";
        // std::cout << "subRouteBCustomer->demand: " << subRouteBCustomer->demand << "\n";

        swap(subRouteA.members[subRouteAMemberIndex], subRouteB.members[subRouteBMemberIndex]);
    }
}