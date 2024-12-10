#ifndef ROUTE_HPP
#define ROUTE_HPP

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
        int size = 1;
        int maxSize = 1;

        double distanceTraveled = -1;
        double timeSpent = -1;
        double duration;

        SubRoute* subRoutes = nullptr;
        DroneRoute droneRoute;

        void finalize();
        void expand();
        void reset();
        void copy(const Route& routeToCopy);

        void shiftLeftSubRoutes(int subRouteOriginIndex);
        void shiftRightSubRoutes(int subRouteOriginIndex);
        void updateDistanceTraveled(int depotIndex);
        void updateTimeSpent(int depotIndex);
        double calculateDuration();

        SubRoute& last() const;
        void print() const;
        void printWithDrone() const;
    private:
        const ProblemInstance* problemInstance;

        void init();
        void resetValues();
        void initializeSubRoutes();
        void initializeNextSubRoute();
        double getComposedDuration(const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex, double truckDuration7);
        double getComposedDeliveryDuration(const Truck& truck, const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex);
        double getComposedMovementDuration(const Truck& truck, const Drone& drone, bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex);
        Sortie* checkSortieVertices(bool* hasConsideredSortie, int subRouteIndex, int sourceIndex, int destIndex);
};

#endif