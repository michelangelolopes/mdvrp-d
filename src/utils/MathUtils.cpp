#include "../../include/utils/MathUtils.hpp"

#include <cmath>

double calculateEuclidianDistance(Position2D positionA, Position2D positionB) {

    return sqrt(pow(positionB.x - positionA.x, 2) + pow(positionB.y - positionA.y, 2));
}