#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.hpp"

#include "DroneRoute.hpp"
#include "SubRoute.hpp"

class Route {
    public:
        Route(const ProblemInstance* problemInstance, int depotIndex) : 
        problemInstance(problemInstance), 
        droneRoute(problemInstance, depotIndex),
        depotIndex(depotIndex)
        {
            init();
        }

        int depotIndex;
        int size;
        int maxSize;

        double distanceTraveled;
        double timeSpent;

        SubRoute* subRoutes;
        DroneRoute droneRoute;

        void finalize();
        void expand();
        void insert(int customerIndex);
        void reset();
        void copy(const Route& routeToCopy);

        void incrementCurrentDuration(double deliveryDuration);
        void incrementCurrentLoad(double demand);
        void shiftLeftSubRoutes(int subRouteOriginIndex);
        void shiftRightSubRoutes(int subRouteOriginIndex);
        void updateDistanceTraveled(int depotIndex);
        void updateTimeSpent(int depotIndex);

        int last() const;
        double currentDuration() const;
        double currentLoad() const;
        void print() const;

    private:
        const ProblemInstance* problemInstance;

        void init();
        void initializeValues();
        void initializeSubRoutes();
        void initializeNextSubRoute();
};

#endif