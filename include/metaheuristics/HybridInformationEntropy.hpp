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

        void finalize() override;
        void update() override;

        void resetEdgesValues() override;
        void updateEdgesOccurrence(const Solution& solution) override;

    protected:
        void init() override;
        double calculate() override;
};

#endif