#ifndef ROUTE_H
#define ROUTE_H

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.h"

#include "SubRoute.h"

class Route {
    public:
        // Route() : 
        // size(1),
        // distanceTraveled(-1),
        // timeSpent(-1)
        // {

        // }
        Route(int subRouteMaxLength) {
            init(subRouteMaxLength);
        }

        int size;
        int maxSize;

        double distanceTraveled;
        double timeSpent;

        SubRoute* subRoutes;

        void finalize();
        void expand();
        void insert(int customerIndex);
        int last();
        void reset();
        void copy(Route routeToCopy);

        double getCurrentLoad();
        void incrementCurrentLoad(double demand);
        void initializeSubRoutes(int subRouteMaxLength);

        void print() const;
        void updateDistanceTraveled(ProblemInstance problemInstance, int depotIndex);
        void updateTimeSpent(ProblemInstance problemInstance, int depotIndex);

    private:
        void init(int subRouteMaxLength);
        void initializeValues();
        void initializeNextSubRoute(int subRouteMaxLength);
};

#endif