#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/Drone.hpp"
#include "../problem/ProblemInstance.hpp"
#include "../problem/Truck.hpp"

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
        double duration;

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
        double calculateDuration();

        SubRoute& last() const;
        int lastCustomer() const;
        double currentDuration() const;
        double currentLoad() const;
        void print() const;
        void printWithDrone() const;
    private:
        const ProblemInstance* problemInstance;

        void init();
        void initializeValues();
        void initializeSubRoutes();
        void initializeNextSubRoute();
        double getComposedDuration(const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex, double truckDuration7);
        double getComposedDeliveryDuration(const Truck& truck, const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex);
        double getComposedMovementDuration(const Truck& truck, const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex);
        Sortie* checkSortieVertices(bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex);
};

#endif