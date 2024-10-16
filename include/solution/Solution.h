#ifndef SOLUTION_H
#define SOLUTION_H

#include "Route.h"

class Solution {
    public:
        Solution(int routesCount, int routeMaxLength) {
            create(routesCount, routeMaxLength);    
        }

        int routesCount;
        int routeMaxLength;
        int** routes;

        void create(int routesCount, int routeMaxLength) {
            this->routesCount = routesCount;
            this->routeMaxLength = routeMaxLength;

            this->routes = (int**) initialize(this->routesCount, this->routeMaxLength, sizeof(int*), sizeof(int));
        }

        void finalize() {
            freeMatrix((void**) routes, routesCount);
        }
};

#endif