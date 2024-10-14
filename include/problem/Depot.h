#ifndef DEPOT_H
#define DEPOT_H

#include <cstdlib>

#include "Position2D.h"
#include "Truck.h"
#include "Uav.h"

class Depot : public Position2D {
    public:
        Truck* trucks = nullptr;
        Uav* uavs = nullptr;

        void finalize() {
            free(this->trucks);
            free(this->uavs);
        }
};

#endif