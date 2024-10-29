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
        int constraints(const ProblemInstance& problemInstance);
        
        void incrementLoad(double demand);
        void updateDistanceTraveled(ProblemInstance problemInstance);
        void updateTimeSpent(ProblemInstance problemInstance, int depotIndex);
        void exchangeMembers(const ProblemInstance& problemInstance, int memberIndexA, int memberIndexB, int successiveVerticesCount);
        void revertExchangeMembers(const ProblemInstance& problemInstance, int memberIndexA, int memberIndexB, int successiveVerticesCount);

        void print() const;
        int first() const;
        int last() const;
    
    private:
        void init();
        void initializeValues();
        void initializeMembers();
};

void exchangeMembersBetweenSubRoutes(
    const ProblemInstance& problemInstance,
    SubRoute& subRouteA,
    SubRoute& subRouteB,
    int memberIndexA,
    int memberIndexB,
    int successiveVerticesCount
);

void revertExchangeMembersBetweenSubRoutes(
    const ProblemInstance& problemInstance,
    SubRoute& subRouteA,
    SubRoute& subRouteB,
    int memberIndexA,
    int memberIndexB,
    int successiveVerticesCount
);

#endif