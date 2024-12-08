#ifndef INFORMATION_ENTROPY_HPP
#define INFORMATION_ENTROPY_HPP

#include "../problem/ProblemInstance.hpp"
#include "../solution/Solution.hpp"

class InformationEntropy {
    public:
        InformationEntropy(const ProblemInstance& problemInstance, int antsCount) : 
        problemInstance(&problemInstance),
        antsCount(antsCount) 
        {
            init();
        }

        int truckEdgesCount = 0;

        double cur = -1;
        double normalized = -1;
        double min = -1;
        double max = -1;
        double coef = 100;

        int antsCount;
        int** truckEdgesOccurrence = nullptr;

        void finalize();
        void update();
        void resetEdgesValues();
        void updateEdgesOccurrence(const Solution& solution);

    protected:
        const ProblemInstance* problemInstance;

        void init();
        double calculate();
};

#endif