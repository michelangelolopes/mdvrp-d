#ifndef INFORMATION_ENTROPY_HPP
#define INFORMATION_ENTROPY_HPP

#include "../solution/Route.hpp"
#include "../solution/SubRoute.hpp"
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
        int droneEdgesCount = 0;

        double cur = -1;
        double normalized = -1;
        double min = -1;
        double max = -1;
        double coef = 100;

        int antsCount;
        int** truckEdgesOccurrence;
        int** droneEdgesOccurrence;

        void finalize();

        void update();
        void updateWithDrone();

        void updateTruckEdgesOccurrence(const Solution& solution);
        void updateDroneEdgesOccurrence(const Solution& solution);

        void resetTruckEdgesValues();
        void resetDroneEdgesValues();

    private:
        const ProblemInstance* problemInstance;

        void init();

        double calculate();
        double calculateWithDrone();
};

#endif