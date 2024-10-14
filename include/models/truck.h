#ifndef TRUCK_H
#define TRUCK_H

#include "vehicle.h"

class Truck : public Vehicle {

    public:

        double routeMaxDuration;
        // Truck() {}

        // Truck(float speedInformed, float capacityInformed, float serviceTimeInformed, float routeMaxDurationInformed) : 
        // Vehicle(speedInformed, capacityInformed, serviceTimeInformed) {
        //     routeMaxDuration = routeMaxDurationInformed;
        // }

        // float getRouteMaxDuration() {
        //     return routeMaxDuration;
        // }

        // void setRouteMaxDuration(float routeMaxDurationInformed) {
        //     routeMaxDuration = routeMaxDurationInformed;
        // }

};

#endif