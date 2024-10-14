#ifndef DEPOT_H
#define DEPOT_H

#include <cstddef>
#include "truck.h"
#include "uav.h"

class Depot {
   
    public:
        double x;
        double y;
        Truck* trucks = NULL;
        Uav* uavs = NULL;
};

#endif