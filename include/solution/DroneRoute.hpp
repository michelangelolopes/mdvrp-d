#ifndef DRONE_ROUTE_HPP
#define DRONE_ROUTE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.hpp"
#include "Sortie.hpp"

class DroneRoute {
    public:
        DroneRoute(const ProblemInstance* problemInstance, int depotIndex) :
        problemInstance(problemInstance),
        depotIndex(depotIndex)
        {
            init();
        }

        int depotIndex;
        int size;

        Sortie* sorties;

        void finalize();
        void reset();
        void copy(const DroneRoute& droneRouteToCopy);
        void insert(const Sortie& sortie);
        void updateRecoveryVertexIndex(int recoveryVertexIndex);
        void print() const;

    private:
        const ProblemInstance* problemInstance;

        void init();
        void initializeValues();
};

#endif