#ifndef SUB_ROUTE_HPP
#define SUB_ROUTE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.hpp"

class SubRoute {
    public:
        SubRoute(const ProblemInstance* problemInstance, int depotIndex, int subRouteIndex, int maxLength) :
        problemInstance(problemInstance),
        depotIndex(depotIndex), 
        subRouteIndex(subRouteIndex), 
        maxLength(maxLength) 
        {
            init();
        }

        int length;
        double duration;
        double load;

        double distanceTraveled;
        double timeSpent;

        int depotIndex;
        int subRouteIndex;
        int maxLength;
        int* members;

        void finalize();
        void insert(int customerIndex);
        void reset();
        void copy(const SubRoute& subRouteToCopy);
        
        void incrementDuration(double deliveryDuration);
        void incrementLoad(double demand);
        void updateDistanceTraveled();
        void updateTimeSpent(int depotIndex);
        void exchangeMembers(int memberIndexA, int memberIndexB, int successiveVerticesCount);
        void revertExchangeMembers(int memberIndexA, int memberIndexB, int successiveVerticesCount);

        int checkWeightConstraint() const;
        int checkTimeConstraint() const;
        void print() const;
        int first() const;
        int last() const;
    
    private:
        const ProblemInstance* problemInstance;

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

void moveMembersBetweenSubRoutes(
    const ProblemInstance& problemInstance,
    SubRoute& subRouteA,
    SubRoute& subRouteB,
    int memberIndexA,
    int memberIndexB,
    int successiveVerticesCount
);

#endif