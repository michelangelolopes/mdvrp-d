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
        Position2D position;
        Truck truck;
        Uav uav;

        void print() {
            position.print();

            std::cout << "\n";

            std::cout << "Truck - ";
            truck.print();
            std::cout << "\n";
            
            std::cout << "Uav - ";
            uav.print();
            std::cout << "\n";
        }
};

#endif