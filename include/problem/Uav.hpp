#ifndef UAV_HPP
#define UAV_HPP

#include <iostream>
#include "Vehicle.hpp"

class Uav : public Vehicle {
    public:
        double endurance;
        double launchTime;
        double recoveryTime;

        inline void print() const {
            std::cout << "Speed: " << speed << " - ";
            std::cout << "Capacity: " << capacity << " - ";
            std::cout << "ServiceTime: " << serviceTime << " - ";
            std::cout << "Endurance: " << endurance << " - ";
            std::cout << "LaunchTime: " << launchTime << " - ";
            std::cout << "RecoveryTime: " << recoveryTime;
        }
};

#endif