#ifndef MAX_MIN_ANT_SYSTEM_H
#define MAX_MIN_ANT_SYSTEM_H

#include "AntSystem.h"

class MaxMinAntSystem : public AntSystem {
    public:
        double pheromoneEvaporationCoefMax;
        double pheromoneEvaporationCoefMin;
};

#endif