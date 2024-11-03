#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.hpp"

#include "SubRoute.hpp"

class Route {
    public:
        Route(int depotIndex, int subRouteMaxLength) : depotIndex(depotIndex) {
            init(subRouteMaxLength);
        }

        int depotIndex;
        int size;
        int maxSize;

        double distanceTraveled;
        double timeSpent;

        SubRoute* subRoutes;

        void finalize();
        void expand();
        void insert(int customerIndex);
        void reset();
        void copy(const Route& routeToCopy);

        void incrementCurrentLoad(double demand);
        void shiftLeftSubRoutes(int subRouteOriginIndex);
        void shiftRightSubRoutes(int subRouteOriginIndex);
        void updateDistanceTraveled(const ProblemInstance& problemInstance, int depotIndex);
        void updateTimeSpent(const ProblemInstance& problemInstance, int depotIndex);

        int last() const;
        double getCurrentLoad() const;
        void print() const;

    private:
        void init(int subRouteMaxLength);
        void initializeValues();
        void initializeSubRoutes(int subRouteMaxLength);
        void initializeNextSubRoute(int subRouteMaxLength);
};

#endif