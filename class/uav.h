#ifndef UAV_H
#define UAV_H

#include "vehicle.h"

class Uav : public Vehicle {
    public:
        double launchTime;
        double recoveryTime;
        double endurance;
};

#endif