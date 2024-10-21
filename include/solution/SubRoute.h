#ifndef SUB_ROUTE_H
#define SUB_ROUTE_H

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.h"

class SubRoute {
    public:
        SubRoute(int maxLength) : maxLength(maxLength) {
            init();
        }

        int length;
        double load;

        double distanceTraveled;
        double timeSpent;

        int maxLength;
        int* members;

        void finalize();
        void insert(int customerIndex);
        void reset();
        void copy(SubRoute subRouteToCopy);
        
        void incrementLoad(double demand);
        void updateDistanceTraveled(ProblemInstance problemInstance);
        void updateTimeSpent(ProblemInstance problemInstance, int depotIndex);

        void print() const;
        int first() const;
        int last() const;
    
    private:
        void init();
        void initializeValues();
        void initializeMembers();
};

#endif