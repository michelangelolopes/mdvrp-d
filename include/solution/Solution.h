#ifndef SOLUTION_H
#define SOLUTION_H

#include <cstdlib>
#include <iostream>
#include "../utils/ArrayUtils.h"
#include "Route.h"

class Solution {
    public:
        Solution(int routesCount, int routeMaxLength) {
            this->routesCount = routesCount;
            create(routeMaxLength);    
        }

        int routesCount;
        double fitness;
        Route* routes;

        void create(int routeMaxLength) {

            this->routes = (Route*) calloc(this->routesCount, sizeof(Route));

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

                if(this->routes[routeIndex].routeRealLength == -1) {
                    std::cout << "not constructed\n";
                    continue;
                }

                for(int visitedVertexIndex = 0; visitedVertexIndex < this->routes[routeIndex].routeRealLength; visitedVertexIndex++) {
                    std::cout << this->routes[routeIndex].visitedVertices[visitedVertexIndex] << " ";
                }
                std::cout << "\n";
            }
            std::cout << "--------------------------------------------------\n";
        }

        void calculateFitness(ProblemInstance problemInstance) {
            double fitness = 0;
            for(int routeIndex = 0; routeIndex < routesCount; routeIndex++) {
                for(int visitedVertexIndex = 0; visitedVertexIndex < routes[routeIndex].routeRealLength - 1; visitedVertexIndex++) {
                    int sourceVertexIndex = routes[routeIndex].visitedVertices[visitedVertexIndex];
                    int destVertexIndex = routes[routeIndex].visitedVertices[visitedVertexIndex + 1];
                    fitness += problemInstance.distanceMatrix[sourceVertexIndex][destVertexIndex];
                }
            }
            this->fitness = fitness;
        }
};

#endif