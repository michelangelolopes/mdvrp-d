#ifndef CUSTOMER_HPP
#define CUSTOMER_HPP

#include <iostream>
#include "../general/Position2D.hpp"

class Customer {
    public:
        Position2D position;
        double demand;

        inline void print() const {
            position.print();
            std::cout << " - Demand: " << demand << "\n";
        }
};

#endif