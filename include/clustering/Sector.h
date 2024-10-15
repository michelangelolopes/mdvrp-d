#ifndef SECTOR_H
#define SECTOR_H

#include "../problem/Position2D.h"

class Sector {
    public:
        Sector() {}
        Sector(Position2D min, Position2D max) : min(min), max(max) {}

        Position2D min;
        Position2D max;
};

#endif