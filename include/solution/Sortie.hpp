#ifndef SORTIE_HPP
#define SORTIE_HPP

#include "../problem/ProblemInstance.hpp"
#include "SubRoute.hpp"

using std::string;
using std::to_string;

class Sortie {
    public:
        Sortie(const ProblemInstance& problemInstance, int launchVertexIndex, int deliveryVertexIndex, int recoveryVertexIndex) : 
        problemInstance(&problemInstance),
        launchVertexIndex(launchVertexIndex),
        deliveryVertexIndex(deliveryVertexIndex),
        recoveryVertexIndex(recoveryVertexIndex) 
        {}

        int launchVertexIndex;
        int deliveryVertexIndex;
        int recoveryVertexIndex;
        int subRouteIndex;
        double duration;

        inline bool isFeasible(const SubRoute& subRoute, int customerIndex) const {

            Truck* truck = &problemInstance->depots[subRoute.depotIndex].truck;
            Drone* drone = &problemInstance->depots[subRoute.depotIndex].drone;
            Customer* customer = &problemInstance->customers[customerIndex];

            double droneDeliveryDuration = problemInstance->calculateDeliveryDuration(*drone, launchVertexIndex, deliveryVertexIndex, recoveryVertexIndex);
            double customerDeliveryDuration = problemInstance->calculateMovementDuration(*truck, launchVertexIndex, recoveryVertexIndex);
            int depotVertexIndex = problemInstance->getDepotVertexIndex(subRoute.depotIndex);
            
            bool willDroneRecoveryBeAtDepot = (recoveryVertexIndex == depotVertexIndex);
            if(!willDroneRecoveryBeAtDepot) {
                customerDeliveryDuration += truck->serviceTime;
            }

            return drone->checkTimeConstraint(customerDeliveryDuration + drone->launchTime + drone->recoveryTime) &&
                drone->checkTimeConstraint(droneDeliveryDuration) &&
                drone->checkWeightConstraint(customer->demand) &&
                truck->checkTimeConstraint(droneDeliveryDuration + subRoute.duration) &&
                truck->checkWeightConstraint(customer->demand + subRoute.load);
        }

        inline bool isSameVertices(int sourceVertexIndex, int destVertexIndex) const {
            return (launchVertexIndex == sourceVertexIndex) && (recoveryVertexIndex == destVertexIndex);
        }

        void print() const {

            std::cout << toString();
        }

        string toString() const {

            return "(" + 
                to_string(launchVertexIndex) + ", " +
                to_string(deliveryVertexIndex) + ", " +
                to_string(recoveryVertexIndex) +
                ")";
        }
    
    private:
        const ProblemInstance* problemInstance;
};

#endif