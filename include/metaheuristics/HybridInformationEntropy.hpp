#ifndef HYBRID_INFORMATION_ENTROPY_HPP
#define HYBRID_INFORMATION_ENTROPY_HPP

#include "InformationEntropy.hpp"

class HybridInformationEntropy : public InformationEntropy {
    public:
        HybridInformationEntropy(const ProblemInstance& problemInstance, int antsCount) : 
        InformationEntropy(problemInstance, antsCount)
        {
            init();
        }

        int droneEdgesCount = 0;
        int** droneEdgesOccurrence = nullptr;

        void finalize();
        void update();

        void resetEdgesValues();
        void updateEdgesOccurrence(const Solution& solution);

    protected:
        void init();
        double calculate();
};

#endif