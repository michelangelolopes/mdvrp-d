#include "../../include/solution/Solution.h"

void Solution::initialize(int subRouteMaxLength) {

    this->routes = (Route*) calloc(this->depotsCount, sizeof(Route));

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        this->routes[depotIndex] = Route(subRouteMaxLength);
    }
}

void Solution::finalize() {

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        this->routes[depotIndex].finalize();
    }

    free(routes);
}

void Solution::updateFitness(ProblemInstance problemInstance) {

    distanceTraveled = 0;
    timeSpent = 0;

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {

        routes[depotIndex].updateTimeSpent(problemInstance, depotIndex);

        distanceTraveled += routes[depotIndex].distanceTraveled;
        timeSpent += routes[depotIndex].timeSpent;
    }

    fitness = timeSpent;
}

void Solution::print() const {

    std::cout << "--------------------------------------------------\n";
    
    if(fitness != -1) {

        std::cout << "Time Spent: " << timeSpent << "\n";

        if(fitness != timeSpent) {
            std::cout << "Fitness: " << fitness << "\n";
        }

        std::cout << "Distance Traveled: " << distanceTraveled << "\n";
    }

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        std::cout << "Route[" << depotIndex << "]: ";
        routes[depotIndex].print();
        std::cout << "\n";
    }

    std::cout << "--------------------------------------------------\n";
}