#ifndef ROUTE_H
#define ROUTE_H

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.h"

#include "SubRoute.h"

class Route {
    public:
        Route(int subRouteMaxLength) {
            initialize(subRouteMaxLength);
        }

        int size = 1;

        double distanceTraveled = -1;
        double timeSpent = -1;

        SubRoute* subRoutes;

        void finalize();
        void expand();
        void shrink();
        void insert(int customerIndex);
        int last();

        double getCurrentLoad();
        void updateCurrentLoad(double demand);

        void print() const;
        void updateDistanceTraveled(ProblemInstance problemInstance, int depotIndex);
        void updateTimeSpent(ProblemInstance problemInstance, int depotIndex);

    private:
        void initialize(int subRouteMaxLength);
        void initializeNextSubRoute(int subRouteMaxLength);
};

#endif