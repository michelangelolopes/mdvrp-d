#ifndef SECTOR_HPP
#define SECTOR_HPP

#include <iostream>
#include "../general/Position2D.hpp"

class Sector {
    public:
        Sector() {}
        Sector(Position2D min, Position2D max) : min(min), max(max) {}

        Position2D min;
        Position2D max;

        void print() {
            min.print();
            std::cout << " - ";
            max.print();
        }
};

#endif