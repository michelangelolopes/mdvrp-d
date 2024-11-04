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