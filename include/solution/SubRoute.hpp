#ifndef SUB_ROUTE_HPP
#define SUB_ROUTE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.hpp"

class SubRoute {
    public:
        SubRoute(int depotIndex, int maxLength) : depotIndex(depotIndex), maxLength(maxLength) {
            init();
        }

        int length;
        double load;

        double distanceTraveled;
        double timeSpent;

        int depotIndex;
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