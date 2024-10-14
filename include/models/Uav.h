#ifndef UAV_H
#define UAV_H

#include "Vehicle.h"

class Uav : public Vehicle {
    public:
        double launchTime;
        double recoveryTime;
        double endurance;
};

#endif