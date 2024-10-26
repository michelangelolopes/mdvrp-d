#ifndef POSITION_2D_HPP
#define POSITION_2D_HPP

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

        inline void print() const {
            std::cout << "(x: " << x << ", y: " << y << ")"; 
        }
};

#endif