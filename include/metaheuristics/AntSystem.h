#ifndef ANT_SYSTEM_H
#define ANT_SYSTEM_H

class AntSystem {
    public:
        int antsCount;
        double pheromoneUpdateCoef;
        double pheromoneProbabilityCoef;
        double distanceProbabilityCoef;
        double** pheromoneMatrix;
};

#endif