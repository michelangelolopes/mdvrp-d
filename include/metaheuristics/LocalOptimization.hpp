#ifndef LOCAL_OPTIMIZATION_HPP
#define LOCAL_OPTIMIZATION_HPP

#include "../solution/Route.hpp"
#include "../solution/SubRoute.hpp"
#include "../solution/Solution.hpp"

class LocalOptimization {
    public:
        LocalOptimization(const ProblemInstance& problemInstance, int maxSuccessiveVertices) : 
        problemInstance(&problemInstance),
        maxSuccessiveVertices(maxSuccessiveVertices)
        {}

        int maxSuccessiveVertices;

        void run(Solution& generationBestSolution);
        void droneLaunchExchange(Route& route);

    private:
        const ProblemInstance* problemInstance;

        void exchangeMembersInSolution(Solution& exchangeSolution);
        void exchangeMembersInRoutes(Solution& exchangeSolution, Route& route, Route& exchangeRoute, int successiveVerticesCount);        
        void exchangeMembersInSameSubRoute(Solution& exchangeSolution, SubRoute& subRoute, int successiveVerticesCount);
        void exchangeMembersInDifferentSubRoutes(Solution& exchangeSolution, SubRoute& subRoute, SubRoute& exchangeSubRoute, int successiveVerticesCount);
        void exchangeMembersBetweenSubRoutes(SubRoute& subRouteA, SubRoute& subRouteB, int memberIndexA,int memberIndexB, int successiveVerticesCount);

        void moveMembersInSolution(Solution& moveSolution);
        void moveMembersInRoutes(Solution& moveSolution, Route& route, Route& destRoute, int successiveVerticesCount);
        bool moveMembersInSubRoutes(Solution& moveSolution, SubRoute& subRoute, SubRoute& destSubRoute, int successiveVerticesCount);
        void moveMembersBetweenSubRoutes(SubRoute& subRouteA, SubRoute& subRouteB, int memberIndexA, int memberIndexB, int successiveVerticesCount);

        void truckDroneExchange(Route& route);
        bool changeSortieLaunch(Route& route, Sortie& sortie, int curVertexIndex, int prevCustomerIndex);
        void droneRecoveryExchange(Route& route);
};

#endif