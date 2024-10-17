#ifndef ROUTE_H
#define ROUTE_H

#include <cstdlib>
#include "../utils/ArrayUtils.h"

class Route {
    public:
        Route(int routeMaxLength) {
            create(routeMaxLength);
        }

        int* visitedVertices;
        int routeRealLength = -1;
        int routeMaxLength;

        void create(int routeMaxLength) {
            this->routeMaxLength = routeMaxLength;

            this->visitedVertices = (int*) calloc(this->routeMaxLength, sizeof(int));
        }

        void finalize() {
            free(visitedVertices);
        }
};

#endif