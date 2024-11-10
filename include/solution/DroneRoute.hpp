#ifndef DRONE_ROUTE_HPP
#define DRONE_ROUTE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.hpp"
#include "Sortie.hpp"

class DroneRoute {
    public:
        DroneRoute(int depotIndex, int maxSize) : 
        depotIndex(depotIndex), 
        maxSize(maxSize) 
        {
            init(maxSize);
        }

        int depotIndex;
        int size;
        int maxSize;

        double distanceTraveled;
        double timeSpent;

        Sortie* sorties;

        void finalize();
        void insert(const Sortie& sortie);
        void reset();
        void updateRecoveryVertexIndex(int recoveryVertexIndex);
        void print() const;

    private:
        void init(int maxSize);
        void initializeValues();
};

#endif