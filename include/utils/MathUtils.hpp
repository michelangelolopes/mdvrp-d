#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>
#include "../general/Position2D.hpp"

inline double calculateEuclidianDistance(Position2D positionA, Position2D positionB) {

    return sqrt(pow(positionB.x - positionA.x, 2) + pow(positionB.y - positionA.y, 2));
}

#endif