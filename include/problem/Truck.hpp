#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <iostream>
#include "Vehicle.hpp"

class Truck : public Vehicle {
    public:
        double routeMaxDuration;

        bool checkTimeConstraint(double duration) const {       

            if(routeMaxDuration == 0) {
                return true;
            }

            return (routeMaxDuration >= duration);
        }

        inline void print() const {
            std::cout << "Speed: " << speed << " - ";
            std::cout << "Capacity: " << capacity << " - ";
            std::cout << "ServiceTime: " << serviceTime << " - ";
            std::cout << "RouteMaxDuration: " << routeMaxDuration;
        }
};

#endif