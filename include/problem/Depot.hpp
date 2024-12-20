#ifndef DEPOT_HPP
#define DEPOT_HPP

#include <cstdlib>
#include <iostream>

#include "../enum/ProblemTypeEnum.hpp"
#include "../general/Position2D.hpp"
#include "Truck.hpp"
#include "Drone.hpp"

#include "../utils/ArrayUtils.hpp"

class Depot {
    public:
        Position2D position;
        Truck truck;
        Drone drone;

        inline void print(const ProblemType& problemType) const {

            position.print();

            std::cout << "\n";

            std::cout << "Truck - ";
            truck.print();
            std::cout << "\n";

            if(problemType == ProblemType::VRP_D || problemType == ProblemType::MDVRP_D) {
                std::cout << "Drone --- ";
                drone.print();
                std::cout << "\n";
            }
        }
};

#endif