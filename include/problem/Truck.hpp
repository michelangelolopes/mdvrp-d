#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <iostream>
#include "Vehicle.hpp"

class Truck : public Vehicle {
    public:
        double routeMaxDuration;

        inline void print() const {
            std::cout << "Speed: " << speed << " - ";
            std::cout << "Capacity: " << capacity << " - ";
            std::cout << "ServiceTime: " << serviceTime << " - ";
            std::cout << "RouteMaxDuration: " << routeMaxDuration;
        }
};

#endif