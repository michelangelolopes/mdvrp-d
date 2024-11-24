#include <iostream>
#include "../../include/metaheuristics/LocalOptimization.hpp"

void LocalOptimization::run(Solution& generationBestSolution) {

    Solution newSolution(*problemInstance);

    newSolution.copy(generationBestSolution);

    exchangeMembersInSolution(newSolution);
    // moveMembersInSolution(newSolution);

    swap(generationBestSolution, newSolution);
    newSolution.finalize();
}

void LocalOptimization::exchangeMembersInSolution(Solution& exchangeSolution) {

    Route* route;
    Route* exchangeRoute;

    for(int successiveVerticesCount = maxSuccessiveVertices; successiveVerticesCount > 0; successiveVerticesCount--) {
        for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
            
            route = &exchangeSolution.routes[depotIndex];

            // std::cout << "depotIndex: [" << depotIndex << "] - size: " << route->size << " - " << exchangeRoute->size << "\n";
            exchangeMembersInRoutes(exchangeSolution, *route, *route, successiveVerticesCount);

            for(int exchangeDepotIndex = depotIndex + 1; exchangeDepotIndex < problemInstance->depotsCount; exchangeDepotIndex++) {

                exchangeRoute = &exchangeSolution.routes[exchangeDepotIndex];

                // std::cout << "depotIndex: [" << depotIndex << "] - ";
                // std::cout << "[" << exchangeDepotIndex << "] - ";
                // std::cout << "size: " << route->size << " - " << exchangeRoute->size << "\n";
                exchangeMembersInRoutes(exchangeSolution, *route, *exchangeRoute, successiveVerticesCount);
            }
        }
    }
}

void LocalOptimization::exchangeMembersInRoutes(Solution& exchangeSolution, Route& route, Route& exchangeRoute, int successiveVerticesCount) {
    
    SubRoute* subRoute;
    SubRoute* exchangeSubRoute;

    int isSameRoute = (route.depotIndex == exchangeRoute.depotIndex);

    for(int subRouteIndex = 0; subRouteIndex < route.size; subRouteIndex++) {

        subRoute = &route.subRoutes[subRouteIndex];
        if(subRoute->length < successiveVerticesCount) {
            break;
        }

        int startExchangeSubRouteIndex = 0;
        if(isSameRoute) {
            startExchangeSubRouteIndex = subRouteIndex + 1;
            exchangeMembersInSameSubRoute(exchangeSolution, *subRoute, successiveVerticesCount);
        }

        for(int exchangeSubRouteIndex = startExchangeSubRouteIndex; exchangeSubRouteIndex < exchangeRoute.size; exchangeSubRouteIndex++) {

            exchangeSubRoute = &exchangeRoute.subRoutes[exchangeSubRouteIndex];
            if(exchangeSubRoute->length < successiveVerticesCount) {
                break;
            }

            // std::cout << "subRoute: [" << subRouteIndex << "] - ";
            // std::cout << "[" << exchangeSubRouteIndex << "] - ";
            // std::cout << "length: " << subRoute->length << " - " << exchangeSubRoute->length << "\n";

            exchangeMembersInDifferentSubRoutes(exchangeSolution, *subRoute, *exchangeSubRoute, successiveVerticesCount);
        }
    }
}

void LocalOptimization::exchangeMembersInSameSubRoute(Solution& exchangeSolution, SubRoute& subRoute, int successiveVerticesCount) {

    double baseFitness = exchangeSolution.fitness;

    for(int memberIndex = 0; memberIndex < subRoute.length; memberIndex++) {

        int maxSuccessiveMembersCount = memberIndex + successiveVerticesCount;
        int willExceedSubRoute = ( maxSuccessiveMembersCount > subRoute.length );
        if(willExceedSubRoute) {
            break;
        }

        for(int exchangeMemberIndex = memberIndex + 1; exchangeMemberIndex < subRoute.length; exchangeMemberIndex++) {
            
            int maxSuccessiveExchangeMembersCount = exchangeMemberIndex + successiveVerticesCount;
            int willExceedExchangeSubRoute = ( maxSuccessiveExchangeMembersCount > subRoute.length );
            if(willExceedExchangeSubRoute) {
                break;
            }

            // std::cout << "member: [" << memberIndex << "] - ";
            // std::cout << "[" << exchangeMemberIndex << "]\n";

            subRoute.exchangeMembers(memberIndex, exchangeMemberIndex, successiveVerticesCount);
            exchangeSolution.updateFitness();

            if(exchangeSolution.fitness < baseFitness) {

                // std::cout << "new.fitness: " << exchangeSolution.fitness << " *improved - ";
                baseFitness = exchangeSolution.fitness;
            } else {

                subRoute.revertExchangeMembers(memberIndex, exchangeMemberIndex, successiveVerticesCount);
                exchangeSolution.updateFitness();                 
            }
        }
    }
}

void LocalOptimization::exchangeMembersInDifferentSubRoutes(Solution& exchangeSolution, SubRoute& subRoute, SubRoute& exchangeSubRoute, int successiveVerticesCount) {

    double baseFitness = exchangeSolution.fitness;

    for(int memberIndex = 0; memberIndex < subRoute.length; memberIndex++) {

        int maxSuccessiveMembersCount = memberIndex + successiveVerticesCount;
        int willExceedSubRoute = ( maxSuccessiveMembersCount > subRoute.length );
        if(willExceedSubRoute) {
            break;
        }

        for(int exchangeMemberIndex = 0; exchangeMemberIndex < exchangeSubRoute.length; exchangeMemberIndex++) {

            int maxSuccessiveExchangeMembersCount = exchangeMemberIndex + successiveVerticesCount;
            int willExceedExchangeSubRoute = ( maxSuccessiveExchangeMembersCount > exchangeSubRoute.length );
            if(willExceedExchangeSubRoute) {
                break;
            }

            // std::cout << "member: [" << memberIndex << "] - ";
            // std::cout << "[" << exchangeMemberIndex << "]\n";

            exchangeMembersBetweenSubRoutes(subRoute, exchangeSubRoute, memberIndex, exchangeMemberIndex, successiveVerticesCount);
            exchangeSolution.updateFitness();

            int isSubRouteContraintSatisfied = subRoute.checkWeightConstraint();
            int isExchangeSubRouteConstraintSatisfied = exchangeSubRoute.checkWeightConstraint();
            int isBetterSolution = (exchangeSolution.fitness < baseFitness);

            if(isSubRouteContraintSatisfied && isExchangeSubRouteConstraintSatisfied && isBetterSolution) {

                // std::cout << "new.fitness: " << exchangeSolution.fitness << " *improved - ";
                baseFitness = exchangeSolution.fitness;
            } else {

                exchangeMembersBetweenSubRoutes(subRoute, exchangeSubRoute, memberIndex, exchangeMemberIndex, successiveVerticesCount);
                exchangeSolution.updateFitness();  
            }
        }
    }
}

void LocalOptimization::exchangeMembersBetweenSubRoutes(SubRoute& subRouteA, SubRoute& subRouteB, int memberIndexA, int memberIndexB, int successiveVerticesCount) {

    for(int successiveIndex = 0; successiveIndex < successiveVerticesCount; successiveIndex++) {

        int subRouteAMemberIndex = memberIndexA + successiveIndex;
        int subRouteBMemberIndex = memberIndexB + successiveIndex;

        int subRouteACustomerIndex = subRouteA.members[subRouteAMemberIndex];
        int subRouteBCustomerIndex = subRouteB.members[subRouteBMemberIndex];

        Customer* subRouteACustomer = &problemInstance->customers[subRouteACustomerIndex];
        Customer* subRouteBCustomer = &problemInstance->customers[subRouteBCustomerIndex];

        subRouteA.load += (subRouteBCustomer->demand - subRouteACustomer->demand);
        subRouteB.load += (subRouteACustomer->demand - subRouteBCustomer->demand);

        // std::cout << "successiveIndex: " << successiveIndex << "\n";
        // std::cout << "subRouteAMemberIndex: " << subRouteAMemberIndex << " - ";
        // std::cout << "subRouteACustomerIndex: " << subRouteACustomerIndex << " - ";
        // std::cout << "subRouteACustomer->demand: " << subRouteACustomer->demand << "\n";
        // std::cout << "subRouteBMemberIndex: " << subRouteBMemberIndex << " - ";
        // std::cout << "subRouteBCustomerIndex: " << subRouteBCustomerIndex << " - ";
        // std::cout << "subRouteBCustomer->demand: " << subRouteBCustomer->demand << "\n";

        swap(subRouteA.members[subRouteAMemberIndex], subRouteB.members[subRouteBMemberIndex]);
    }
}

void LocalOptimization::moveMembersInSolution(Solution& moveSolution) {

    Route* route;
    Route* destRoute;

    for(int successiveVerticesCount = maxSuccessiveVertices; successiveVerticesCount > 0; successiveVerticesCount--) {
        for(int depotIndex = 0; depotIndex < problemInstance->depotsCount; depotIndex++) {
            
            route = &moveSolution.routes[depotIndex];

            for(int destDepotIndex = 0; destDepotIndex < problemInstance->depotsCount; destDepotIndex++) {

                destRoute = &moveSolution.routes[destDepotIndex];

                // std::cout << "depotIndex: [" << depotIndex << "] - ";
                // std::cout << "[" << destDepotIndex << "] - ";
                // std::cout << "size: " << route->size << " - " << destRoute->size << "\n";
                moveMembersInRoutes(moveSolution, *route, *destRoute, successiveVerticesCount);
            }
        }
    }
}

void LocalOptimization::moveMembersInRoutes(Solution& moveSolution, Route& route, Route& destRoute, int successiveVerticesCount) {
    
    SubRoute* subRoute;
    SubRoute* destSubRoute;

    int isSameRoute = (route.depotIndex == destRoute.depotIndex);
    int isSameSubRoute;

    int subRouteIndex = 0;
    int destSubRouteIndex = 0;

    while(subRouteIndex < route.size) {

        subRoute = &route.subRoutes[subRouteIndex];
        if(subRoute->length < successiveVerticesCount) {
            subRouteIndex++;
            continue;
        }

        int hasLeftShiftedSubRoute = 0;
        while(!hasLeftShiftedSubRoute && destSubRouteIndex < destRoute.size) {

            destSubRoute = &destRoute.subRoutes[destSubRouteIndex];

            isSameSubRoute = isSameRoute && (subRoute->subRouteIndex == destSubRoute->subRouteIndex);
            if(isSameSubRoute) {
                destSubRouteIndex++;
                continue;
            }

            // std::cout << "subRoute: [" << subRouteIndex << "] - ";
            // std::cout << "[" << destSubRouteIndex << "] - ";
            // std::cout << "length: " << subRoute->length << " - " << destSubRoute->length << "\n";

            hasLeftShiftedSubRoute = moveMembersInSubRoutes(moveSolution, *subRoute, *destSubRoute, successiveVerticesCount);
            destSubRouteIndex++;
        }

        if(!hasLeftShiftedSubRoute) {
            subRouteIndex++;
        }
    }
}

bool LocalOptimization::moveMembersInSubRoutes(Solution& moveSolution, SubRoute& subRoute, SubRoute& destSubRoute, int successiveVerticesCount) {

    auto willSuccessiveExceedSubRoute = [successiveVerticesCount](int memberIndex, SubRoute& subRoute) {
        return (memberIndex + successiveVerticesCount) > subRoute.length;
    };
    auto willExceedSubRoute = [](int memberIndex, SubRoute& subRoute) {
        return memberIndex > subRoute.length;
    };

    double baseFitness = moveSolution.fitness;
    int memberIndex = 0;
    int destMemberIndex = 0;

    Route* route = &moveSolution.routes[subRoute.depotIndex];
    Route* destRoute = &moveSolution.routes[destSubRoute.depotIndex];

    while( !willSuccessiveExceedSubRoute(memberIndex, subRoute) ) {

        int isBetterSolution = 0;
        while( !isBetterSolution && !willExceedSubRoute(destMemberIndex, destSubRoute) ) {

            // std::cout << "member: [" << memberIndex << "] - ";
            // std::cout << "[" << destMemberIndex << "]\n";

            moveMembersBetweenSubRoutes(subRoute, destSubRoute, memberIndex, destMemberIndex, successiveVerticesCount);

            int hasLeftShiftedSubRoute = (subRoute.length == 0);
            if(hasLeftShiftedSubRoute) {
                // std::cout << "RemovedSubRoute: " << subRoute.subRouteIndex << "\n";
                // std::cout << "Before - route->size: " << route->size << "\n";
                route->shiftLeftSubRoutes(subRoute.subRouteIndex);
                // std::cout << "After - route->size: " << route->size << "\n";
            }

            moveSolution.updateFitness();

            int isSubRouteContraintSatisfied = subRoute.checkWeightConstraint();
            int isDestSubRouteConstraintSatisfied = destSubRoute.checkWeightConstraint();

            isBetterSolution = isSubRouteContraintSatisfied && isDestSubRouteConstraintSatisfied && (moveSolution.fitness < baseFitness);
            if(isBetterSolution) {

                // std::cout << "new.fitness: " << moveSolution.fitness << " *improved\n";
                baseFitness = moveSolution.fitness;

                if(hasLeftShiftedSubRoute) {
                    return 1;
                }

                memberIndex--;
            } else {

                if(hasLeftShiftedSubRoute) {
                    route->shiftRightSubRoutes(subRoute.subRouteIndex);
                }

                moveMembersBetweenSubRoutes(destSubRoute, subRoute, destMemberIndex, memberIndex, successiveVerticesCount);
                moveSolution.updateFitness();

                destMemberIndex++;
            }
        }

        memberIndex++;
    }

    return 0;
}

void LocalOptimization::moveMembersBetweenSubRoutes(SubRoute& subRouteA, SubRoute& subRouteB, int memberIndexA, int memberIndexB, int successiveVerticesCount) {

    // std::cout << "--------------------------\nMove.Before\n";
    // subRouteA.print();
    // std::cout << "\n";
    // subRouteB.print();
    // std::cout << "\n";

    shiftRightArray(subRouteB.members, subRouteB.length, memberIndexB, successiveVerticesCount);
    subRouteB.length += successiveVerticesCount;

    for(int successiveIndex = 0; successiveIndex < successiveVerticesCount; successiveIndex++) {

        int subRouteAMemberIndex = memberIndexA + successiveIndex;
        int subRouteBMemberIndex = memberIndexB + successiveIndex;

        int subRouteACustomerIndex = subRouteA.members[subRouteAMemberIndex];
        Customer* subRouteACustomer = &problemInstance->customers[subRouteACustomerIndex];

        subRouteB.members[subRouteBMemberIndex] = subRouteACustomerIndex;

        subRouteA.load -= subRouteACustomer->demand;
        subRouteB.load += subRouteACustomer->demand;
    }

    shiftLeftArray(subRouteA.members, subRouteA.length, memberIndexA, successiveVerticesCount);
    subRouteA.length -= successiveVerticesCount;

    // std::cout << "Move.After\n";
    // subRouteA.print();
    // std::cout << "\n";
    // subRouteB.print();
    // std::cout << "\n";
}

void LocalOptimization::truckDroneExchange(Route& route) {

    Route routeTemp(problemInstance, route.depotIndex);
    routeTemp.copy(route);

    DroneRoute droneRouteTemp(problemInstance, route.depotIndex);
    droneRouteTemp.copy(route.droneRoute);

    for(int subRouteIndex = 0; subRouteIndex < route.size; subRouteIndex++) {
        
        SubRoute* subRoute = &route.subRoutes[subRouteIndex];
        for(int memberIndex = 0; memberIndex < subRoute->length; memberIndex++) {
            int customerIndex = subRoute->members[memberIndex];

            // same subroute
        }
    }
}

void LocalOptimization::droneLaunchExchange(Route& route) {

    // std::cout << "Before: " << route.duration << endl;
    // route.printWithDrone();
    // std::cout << endl;

    Truck* truck = &problemInstance->depots[route.depotIndex].truck;
    Drone* drone = &problemInstance->depots[route.depotIndex].drone;

    DroneRoute droneRouteTemp(problemInstance, route.depotIndex);
    droneRouteTemp.copy(route.droneRoute);

    int depotVertexIndex = problemInstance->getDepotVertexIndex(route.depotIndex);

    for(int sortieIndex = 0; sortieIndex < droneRouteTemp.size; sortieIndex++) {
        
        Sortie* sortie = &droneRouteTemp.sorties[sortieIndex];

        // std::cout << "A: " << sortie->toString() << endl;

        for(int subRouteIndex = sortie->subRouteIndex; subRouteIndex >= 0; subRouteIndex--) {
            
            SubRoute* subRoute = &route.subRoutes[subRouteIndex];
            SubRoute* prevSubRoute = nullptr;

            if(subRouteIndex > 0) {
                prevSubRoute = &route.subRoutes[subRouteIndex - 1];
            }

            if(sortie->launchVertexIndex == depotVertexIndex && prevSubRoute != nullptr) {
                changeSortieLaunch(route, *sortie, depotVertexIndex, prevSubRoute->last());
                continue;
            }

            for(int memberIndex = subRoute->length - 1; memberIndex > 0; memberIndex--) {

                int customerIndex = subRoute->members[memberIndex];
                int prevCustomerIndex = subRoute->members[memberIndex - 1];
                
                if(customerIndex == sortie->launchVertexIndex) {
                    changeSortieLaunch(route, *sortie, customerIndex, prevCustomerIndex);
                }
            }
        }
    }
}

bool LocalOptimization::changeSortieLaunch(Route& route, Sortie& sortie, int curVertexIndex, int prevCustomerIndex) {

    Truck* truck = &problemInstance->depots[route.depotIndex].truck;
    Drone* drone = &problemInstance->depots[route.depotIndex].drone;
    Customer* customer = &problemInstance->customers[prevCustomerIndex];

    sortie.launchVertexIndex = prevCustomerIndex;
    // std::cout << "B: " << sortie.toString() << endl;

    bool isChangeFeasible = sortie.isFeasible(route.last(), prevCustomerIndex);
    // std::cout << "isChangeFeasible: " << (isChangeFeasible ? "true" : "false") << endl;
    
    if(isChangeFeasible) {

        double baseDuration = route.duration;
        route.calculateDuration();

        // std::cout << "After: " << route.duration << endl;
        // route.printWithDrone();
        // std::cout << endl;

        if(route.duration < baseDuration) {
            // std::cout << "baseDuration: " << baseDuration << endl;
            // std::cout << "newDuration.: " << route.duration << endl;
            return true;
        } else {
            sortie.launchVertexIndex = curVertexIndex;
            route.duration = baseDuration;
        }
    }

    return false;
}


void droneRecoveryExchange(Route& route) {}

