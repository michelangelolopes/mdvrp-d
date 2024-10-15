#ifndef UAV_H
#define UAV_H

#include <iostream>
#include "Vehicle.h"

class Uav : public Vehicle {
    public:
        double endurance;
        double launchTime;
        double recoveryTime;

        void print() {
            std::cout << "Speed: " << speed << " - ";
            std::cout << "Capacity: " << capacity << " - ";
            std::cout << "ServiceTime: " << serviceTime << " - ";
            std::cout << "Endurance: " << endurance << " - ";
            std::cout << "LaunchTime: " << launchTime << " - ";
            std::cout << "RecoveryTime: " << recoveryTime;
        }
};

#endif