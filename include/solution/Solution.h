#ifndef SOLUTION_H
#define SOLUTION_H

#include <cstdlib>
#include <iostream>
#include "../utils/ArrayUtils.h"
#include "Route.h"

class Solution {
    public:
        Solution(int routesCount, int routeMaxLength) {
            create(routesCount, routeMaxLength);    
        }

        int routesCount;
        double fitness;
        Route* routes;

        void create(int routesCount, int routeMaxLength) {
            this->routesCount = routesCount;

            this->routes = (Route*) initialize(this->routesCount, sizeof(Route));

            for(int routeIndex = 0; routeIndex < routesCount; routeIndex++) {
                this->routes[routeIndex] = Route(routeMaxLength);
            }
        }

        void finalize() {
            for(int routeIndex = 0; routeIndex < routesCount; routeIndex++) {
                this->routes[routeIndex].finalize();
            }

            free(routes);
        }

        void print() {
            std::cout << "--------------------------------------------------\n";
            std::cout << "Fitness: " << fitness << "\n";
            for(int routeIndex = 0; routeIndex < routesCount; routeIndex++) {
                std::cout << "Route[" << routeIndex << "]: ";
                for(int visitedVertexIndex = 0; visitedVertexIndex < this->routes[routeIndex].routeRealLength; visitedVertexIndex++) {
                    std::cout << this->routes[routeIndex].visitedVertices[visitedVertexIndex] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "--------------------------------------------------\n";
        }
};

#endif