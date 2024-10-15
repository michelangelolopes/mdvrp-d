#ifndef POSITION_2D_H
#define POSITION_2D_H

#include <iostream>

class Position2D {
    public:
        Position2D() {}
        Position2D(double x, double y) {
            this->x = x;
            this->y = y;
        }

        double x;
        double y;

        void print() {
            std::cout << "(x: " << x << ", y: " << y << ")"; 
        }
};

#endif