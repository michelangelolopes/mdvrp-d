#ifndef DEPOT_HPP
#define DEPOT_HPP

#include <cstdlib>
#include <iostream>

#include "../general/Position2D.hpp"
#include "Truck.hpp"
#include "Uav.hpp"

#include "../utils/ArrayUtils.hpp"

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