#include "../../include/solution/Solution.hpp"

void Solution::init(int subRouteMaxLength) {

    int maxSize = (subRouteMaxLength / 2) + 1;

    initializeValues();
    initializeRoutes(subRouteMaxLength);
    initializeDroneRoutes(maxSize);
}

void Solution::initializeValues() {

    fitness = -1;
    totalDistanceTraveled = -1;
    totalTimeSpent = -1;
    maxDistanceTraveled = -1;
    maxTimeSpent = -1;
}

void Solution::initializeRoutes(int subRouteMaxLength) {

    routes = (Route*) malloc(depotsCount * sizeof(Route));

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        routes[depotIndex] = Route(depotIndex, subRouteMaxLength);
    }
}

void Solution::initializeDroneRoutes(int maxSize) {

    droneRoutes = (DroneRoute*) malloc(depotsCount * sizeof(DroneRoute));

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        droneRoutes[depotIndex] = DroneRoute(depotIndex, maxSize);
    }
}

void Solution::finalize() {

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        routes[depotIndex].finalize();
    }

    free(routes);

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        droneRoutes[depotIndex].finalize();
    }

    free(droneRoutes);
}

void Solution::reset() {

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        routes[depotIndex].reset();
        droneRoutes[depotIndex].reset();
    }

    initializeValues();
}

void Solution::copy(Solution solutionToCopy) {

    fitness = solutionToCopy.fitness;
    totalDistanceTraveled = solutionToCopy.totalDistanceTraveled;
    totalTimeSpent = solutionToCopy.totalTimeSpent;
    maxDistanceTraveled = solutionToCopy.maxDistanceTraveled;
    maxTimeSpent = solutionToCopy.maxTimeSpent;

    for(int depotIndex = 0; depotIndex < solutionToCopy.depotsCount; depotIndex++) {
        routes[depotIndex].copy(solutionToCopy.routes[depotIndex]);
    }
}

void Solution::updateFitness(const ProblemInstance& problemInstance) {

    totalDistanceTraveled = 0;
    totalTimeSpent = 0;
    maxDistanceTraveled = -1;
    maxTimeSpent = -1;

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {

        routes[depotIndex].updateTimeSpent(problemInstance, depotIndex);

        double distanceTraveledInRoute = routes[depotIndex].distanceTraveled;
        double timeSpentInRoute = routes[depotIndex].timeSpent;

        if(maxDistanceTraveled == -1 || maxDistanceTraveled < distanceTraveledInRoute) {
            maxDistanceTraveled = distanceTraveledInRoute;
        }

        if(maxTimeSpent == -1 || maxTimeSpent < timeSpentInRoute) {
            maxTimeSpent = timeSpentInRoute;
        }

        totalDistanceTraveled += distanceTraveledInRoute;
        totalTimeSpent += timeSpentInRoute;
    }

    switch(minimizationType) {
        case TOTAL_DISTANCE_TRAVELED:
            fitness = totalDistanceTraveled;
            break;
        case TOTAL_TIME_SPENT:
            fitness = totalTimeSpent;
            break;
        case MAX_DISTANCE_TRAVELED:
            fitness = maxDistanceTraveled;
            break;
        case MAX_TIME_SPENT:
            fitness = maxTimeSpent;
            break;
    }
}

void Solution::updateFitnessWithDrone(const ProblemInstance& problemInstance) {

    totalDistanceTraveled = 0;
    totalTimeSpent = 0;
    maxDistanceTraveled = -1;
    maxTimeSpent = -1;

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {

        Route* route = &routes[depotIndex];
        double routeDuration = 0;

        for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {
            SubRoute* subRoute = &route->subRoutes[subRouteIndex];
            routeDuration += subRoute->duration;
        }

        if(maxTimeSpent == -1 || maxTimeSpent < routeDuration) {
            maxTimeSpent = routeDuration;
        }
    }

    fitness = maxTimeSpent;
}

void Solution::print() const {

    std::cout << "--------------------------------------------------\n";
    
    if(fitness != -1) {

        switch(minimizationType) {
            case TOTAL_DISTANCE_TRAVELED:
                std::cout << "Total Distance Traveled: " << totalDistanceTraveled << "\n";
                break;
            case TOTAL_TIME_SPENT:
                std::cout << "Total Time Spent: " << totalTimeSpent << "\n";
                break;
            case MAX_DISTANCE_TRAVELED:
                std::cout << "Max Distance Traveled: " << maxDistanceTraveled << "\n";
                break;
            case MAX_TIME_SPENT:
                std::cout << "Max Time Spent: " << maxTimeSpent << "\n";
                break;
            }
    }

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        std::cout << "Route[" << depotIndex << "]: ";
        routes[depotIndex].print();
        std::cout << "\n";
    }

    std::cout << "--------------------------------------------------\n";
}

void Solution::printWithDrone() const {

    std::cout << "--------------------------------------------------\n";
    
    if(fitness != -1) {

        switch(minimizationType) {
            case TOTAL_DISTANCE_TRAVELED:
                std::cout << "Total Distance Traveled: " << totalDistanceTraveled << "\n";
                break;
            case TOTAL_TIME_SPENT:
                std::cout << "Total Time Spent: " << totalTimeSpent << "\n";
                break;
            case MAX_DISTANCE_TRAVELED:
                std::cout << "Max Distance Traveled: " << maxDistanceTraveled << "\n";
                break;
            case MAX_TIME_SPENT:
                std::cout << "Max Time Spent: " << maxTimeSpent << "\n";
                break;
            }
    }

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        std::cout << "Route[" << depotIndex << "]: ";
        printWithDrone(depotIndex);
        std::cout << "\n";
    }

    std::cout << "--------------------------------------------------\n";
}

void Solution::printWithDrone(int depotIndex) const {

    Route* route = &routes[depotIndex];
    DroneRoute* droneRoute = &droneRoutes[depotIndex];

    // droneRoute->print();

    int sortieIndex = 0;
    Sortie* sortie = &droneRoute->sorties[sortieIndex];
    int depotVertexIndex = depotIndex + route->subRoutes[0].maxLength;
    int customersInRouteCount = 0;
    bool hasDroneRouteEnded = (sortieIndex >= droneRoute->size);

    for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {
        
        std::cout << "# ";
        SubRoute* subRoute = &route->subRoutes[subRouteIndex];
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

            hasDroneRouteEnded = (sortieIndex >= droneRoute->size);
            if(!hasDroneRouteEnded) {
                sortie = &droneRoute->sorties[sortieIndex];
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
                // std::cout << "droneRoute->size: " << droneRoute->size << endl;

                std::cout << " ";
                std::cout << "D" << sortie->deliveryVertexIndex;
                sortieIndex++;
                
                hasDroneRouteEnded = (sortieIndex >= droneRoute->size);
                if(!hasDroneRouteEnded) {
                    sortie = &droneRoute->sorties[sortieIndex];
                }
            }

            if(memberIndex != subRoute->length - 1) {
                std::cout << " ";
            }
        }

        if(subRouteIndex != route->size - 1) {
            std::cout << " ";
        }
    }

    std::cout << " #";
    std::cout << " - (" << customersInRouteCount;
    std::cout << ", " << droneRoute->size << ")";
}

double Solution::calculateRouteDuration(const ProblemInstance& problemInstance, int depotIndex) {

    int depotVertexIndex = problemInstance.getDepotVertexIndex(depotIndex);

    Route* route = &routes[depotIndex];
    DroneRoute* droneRoute = &droneRoutes[depotIndex];

    Truck* truck = &problemInstance.depots[depotIndex].truck;
    Drone* drone = &problemInstance.depots[depotIndex].drone;

    int sortieIndex = 0;
    Sortie* sortie = &droneRoute->sorties[sortieIndex];
    double routeDuration = 0;
    bool hasDroneRouteEnded = (sortieIndex >= droneRoute->size);

    for(int subRouteIndex = 0; subRouteIndex < route->size; subRouteIndex++) {

        SubRoute* subRoute = &route->subRoutes[subRouteIndex];

        double customerDeliveryDuration = problemInstance.calculateDeliveryDuration(*truck, depotVertexIndex, subRoute->first());
        if(!hasDroneRouteEnded && sortie->launchVertexIndex == depotVertexIndex && sortie->recoveryVertexIndex == subRoute->first()) { //launched at depot

            double truckFullDuration = customerDeliveryDuration + drone->launchTime + drone->recoveryTime;
            double droneDeliveryDuration = problemInstance.calculateDeliveryDuration(*drone, *sortie);
            double vehicleLongestDuration = max(truckFullDuration, droneDeliveryDuration);

            routeDuration += vehicleLongestDuration;

            sortieIndex++;
            hasDroneRouteEnded = (sortieIndex >= droneRoute->size);
            if(!hasDroneRouteEnded) {
                sortie = &droneRoute->sorties[sortieIndex];
            }
        } else {
            routeDuration += customerDeliveryDuration;
        }

        for(int memberIndex = 0; memberIndex < subRoute->length; memberIndex++) {
            int customerIndex = subRoute->members[memberIndex];
            int neighborCustomerIndex;

            double customerDeliveryDuration;
            if(memberIndex + 1 < subRoute->length) {
                neighborCustomerIndex = subRoute->members[memberIndex + 1];
                customerDeliveryDuration = problemInstance.calculateDeliveryDuration(*truck, customerIndex, neighborCustomerIndex);
            } else {
                neighborCustomerIndex = depotVertexIndex;
                customerDeliveryDuration = problemInstance.calculateMovementDuration(*truck, customerIndex, neighborCustomerIndex);
            }
            
            // std::cout << to_string(customerIndex) + " -> " + to_string(neighborCustomerIndex) + " = " + to_string(customerDeliveryDuration) + "\n";

            if(!hasDroneRouteEnded && sortie->launchVertexIndex == customerIndex) {

                double truckFullDuration = customerDeliveryDuration + drone->launchTime + drone->recoveryTime;
                double droneDeliveryDuration = problemInstance.calculateDeliveryDuration(*drone, *sortie);
                double vehicleLongestDuration = max(truckFullDuration, droneDeliveryDuration);

                routeDuration += vehicleLongestDuration;

                sortieIndex++;
                hasDroneRouteEnded = (sortieIndex >= droneRoute->size);
                if(!hasDroneRouteEnded) {
                    sortie = &droneRoute->sorties[sortieIndex];
                }
            } else {
                routeDuration += customerDeliveryDuration;
            }
        }

    }

    return routeDuration;
}