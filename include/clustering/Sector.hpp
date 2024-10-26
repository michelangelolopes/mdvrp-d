#ifndef SECTOR_HPP
#define SECTOR_HPP

#include <iostream>
#include "../general/Position2D.hpp"

class Sector {
    public:
        Sector() {}
        Sector(const Position2D& min, const Position2D& max) : min(min), max(max) {}

        Position2D min;
        Position2D max;

        inline void print() const {
            min.print();
            std::cout << " - ";
            max.print();
        }

        inline int contains(const Position2D& position) const {
            return (position.x >= min.x) && 
                (position.x <= max.x) && 
                (position.y >= min.y) && 
                (position.y <= max.y);
        }
};

#endif