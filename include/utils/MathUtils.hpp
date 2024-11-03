#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cmath>    // pow(), sqrt()
#include <cstdlib>  // rand()
#include "../general/Position2D.hpp"
#include "../utils/ArrayUtils.hpp"

inline double add(double x, double y) { 
    return x + y; 
}

inline double multiply(double x, double y) {
    return x * y; 
}

inline double calculateEuclidianDistance(Position2D positionA, Position2D positionB) {

    return sqrt(pow(positionB.x - positionA.x, 2) + pow(positionB.y - positionA.y, 2));
}

inline void normalizeValues(double* values, int count) {
    
    double sum = sumArray(values, count);

    for(int i = 0; i < count; i++) {
        values[i] /= sum;
    }
}

inline int rouletteWheelSelection(double* candidatesSelectionProbability, int candidatesCount) {
    
    normalizeValues(candidatesSelectionProbability, candidatesCount);
    
    double randomValue = ((double)rand() / RAND_MAX);
    
    double cumulativeProbability = 0;
    for(int index = 0; index < candidatesCount; index++) {

        cumulativeProbability += candidatesSelectionProbability[index];
        if (randomValue <= cumulativeProbability) {
            return index;
        }
    }

    return -1;
}

#endif