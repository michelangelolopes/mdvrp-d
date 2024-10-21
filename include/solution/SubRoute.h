#ifndef SUB_ROUTE_H
#define SUB_ROUTE_H

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.h"

class SubRoute {
    public:
        SubRoute(int maxLength) : maxLength(maxLength) {
            initialize();
        }

        int length = 0;
        double load = 0;

        double distanceTraveled = -1;
        double timeSpent = -1;

        int maxLength;
        int* members;

        void finalize();
        void shrink();
        void insert(int customerIndex);
        
        void incrementLoad(double demand);
        void updateDistanceTraveled(ProblemInstance problemInstance);
        void updateTimeSpent(ProblemInstance problemInstance, int depotIndex);

        void print() const;
        int first() const;
        int last() const;
    
    private:
        void initialize();
};

#endif