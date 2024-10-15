#ifndef TRUCK_H
#define TRUCK_H

#include <iostream>
#include "Vehicle.h"

class Truck : public Vehicle {
    public:
        double routeMaxDuration;

        void print() {
            std::cout << "Speed: " << speed << " - ";
            std::cout << "Capacity: " << capacity << " - ";
            std::cout << "ServiceTime: " << serviceTime << " - ";
            std::cout << "RouteMaxDuration: " << routeMaxDuration;
        }
};

#endif