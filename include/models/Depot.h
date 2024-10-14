#ifndef DEPOT_H
#define DEPOT_H

#include "Position2D.h"
#include "Truck.h"
#include "Uav.h"

class Depot : public Position2D {
    public:
        Truck* trucks = nullptr;
        Uav* uavs = nullptr;
};

#endif