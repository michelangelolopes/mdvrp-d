#ifndef DRONE_ROUTE_HPP
#define DRONE_ROUTE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.hpp"
#include "Sortie.hpp"

class DroneRoute {
    public:
        DroneRoute(const ProblemInstance* problemInstance, int depotIndex) : problemInstance(problemInstance), depotIndex(depotIndex)
        {
            init();
        }

        int depotIndex;
        int size;

        double distanceTraveled;
        double timeSpent;

        Sortie* sorties;

        void finalize();
        void insert(const Sortie& sortie);
        void reset();
        void updateRecoveryVertexIndex(int recoveryVertexIndex);
        void print() const;

    private:
        const ProblemInstance* problemInstance;

        void init();
        void initializeValues();
};

#endif