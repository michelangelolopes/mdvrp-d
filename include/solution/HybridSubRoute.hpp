#ifndef HYBRID_SUB_ROUTE_HPP
#define HYBRID_SUB_ROUTE_HPP

#include "Sortie.hpp"
#include "SubRoute.hpp"

class HybridSubRoute : public SubRoute {
    public:
        HybridSubRoute(const ProblemInstance* problemInstance, int depotIndex, int subRouteIndex) :
        SubRoute(problemInstance, depotIndex, subRouteIndex)
        {
            init();
        }

        int sortiesCount = 0;
        Sortie* sorties = nullptr;

        void resetValues();
        void finalize();
        void copy(const HybridSubRoute& subRouteToCopy);

        void insertSortie(const Sortie& sortie);
        Sortie& lastSortie() const;
        void printSorties() const;
    
    private:
        void init();
};

#endif