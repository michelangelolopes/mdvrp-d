#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>
#include "../general/Position2D.h"

class Customer {
    public:
        Position2D position;
        double demand;

        void print() {
            position.print();
            std::cout << " - Demand: " << demand << "\n";
        }
};

#endif