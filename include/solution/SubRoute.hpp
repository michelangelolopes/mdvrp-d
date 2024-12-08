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

        int length;
        double duration;
        double load;

        double distanceTraveled;
        double timeSpent;

        int depotIndex;
        int subRouteIndex;
        int* members = nullptr;

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
};

#endif