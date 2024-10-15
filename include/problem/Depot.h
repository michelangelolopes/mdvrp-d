#ifndef DEPOT_H
#define DEPOT_H

#include <cstdlib>
#include <iostream>

#include "Position2D.h"
#include "Truck.h"
#include "Uav.h"

#include "../utils/ArrayUtils.h"

class Depot {
    public:
        Depot(int vehicleCount) {            
            create(vehicleCount);
        }

        int vehicleCount;
        Position2D position;
        Truck* trucks = nullptr;
        Uav* uavs = nullptr;

        void create(int vehicleCount) {
            this->vehicleCount = vehicleCount;
            this->trucks = (Truck*) initialize(vehicleCount, sizeof(Truck));
            this->uavs = (Uav*) initialize(vehicleCount, sizeof(Uav));
        }

        void finalize() {
            free(this->trucks);
            free(this->uavs);
        }

        void print() {
            position.print();

            std::cout << " - VehicleCount: " << vehicleCount << "\n";

            for(int vehicleIndex = 0; vehicleIndex < vehicleCount; vehicleIndex++) {
                std::cout << "Truck - ";
                trucks[vehicleIndex].print();
                std::cout << "\n";
            }

            for(int vehicleIndex = 0; vehicleIndex < vehicleCount; vehicleIndex++) {
                std::cout << "Uav - ";
                uavs[vehicleIndex].print();
                std::cout << "\n";
            }
        }
};

#endif