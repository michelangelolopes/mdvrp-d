#ifndef SUB_ROUTE_HPP
#define SUB_ROUTE_HPP

#include "../problem/ProblemInstance.hpp"

class SubRoute {
    public:
        SubRoute(const ProblemInstance* problemInstance, int depotIndex, int subRouteIndex) :
        problemInstance(problemInstance),
        depotIndex(depotIndex), 
        subRouteIndex(subRouteIndex)
        {
            init();
        }

        int length = 0;
        double duration = 0;
        double load = 0;

        double distanceTraveled = -1;
        double timeSpent = -1;

        int depotIndex;
        int subRouteIndex;
        int* members = nullptr;

        void resetValues();
        void finalize();
        void insert(int customerIndex);
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
    
    protected:
        const ProblemInstance* problemInstance;

    private:
        void init();
};

#endif