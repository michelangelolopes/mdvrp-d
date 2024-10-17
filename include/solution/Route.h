#ifndef ROUTE_H
#define ROUTE_H

#include <cstdlib>
#include "../utils/ArrayUtils.h"

class Route {
    public:
        Route(int routeMaxLength) {
            this->routeMaxLength = routeMaxLength;
            create();
        }

        int* visitedVertices;
        int routeRealLength = -1;
        int routeMaxLength;

        void create() {
            this->visitedVertices = (int*) calloc(this->routeMaxLength, sizeof(int));
        }

        void finalize() {
            free(visitedVertices);
        }
};

#endif