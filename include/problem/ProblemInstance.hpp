#ifndef PROBLEM_INSTANCE_HPP
#define PROBLEM_INSTANCE_HPP

#include <fstream>
#include <string>

#include "../enum/MinimizationTypeEnum.hpp"
#include "../enum/ProblemTypeEnum.hpp"

#include "Customer.hpp"
#include "Depot.hpp"

using std::string;

class ProblemInstance {
    public:
        ProblemInstance(const string& datasetFilename, const ProblemType& problemType) : 
        problemType(problemType)
        {
            create(datasetFilename);
        }

        ProblemType problemType;
        MinimizationType minimizationType;
        
        Depot* depots = nullptr;
        Customer* customers = nullptr;
        double** verticesDistanceMatrix = nullptr;

        int depotsCount;
        int customersCount;
        int verticesCount;

        void finalize();

        void print(int printDistanceMatrix) const;

        inline double calculateDeliveryDuration(const Truck& truck, int sourceIndex, int destIndex) const {
            return calculateMovementDuration(truck, sourceIndex, destIndex) + truck.serviceTime;
        }

        inline double calculateDeliveryDuration(const Drone& drone, int launchIndex, int deliveryIndex, int recoveryIndex) const {

            return calculateMovementDuration(drone, launchIndex, deliveryIndex) + 
                calculateMovementDuration(drone, deliveryIndex, recoveryIndex) +
                drone.launchTime +
                drone.recoveryTime +
                drone.serviceTime;
        }

        inline double calculateMovementDuration(const Vehicle& vehicle, int sourceIndex, int destIndex) const {
            return verticesDistanceMatrix[sourceIndex][destIndex] / vehicle.speed;
        }

        inline int getDepotVertexIndex(int depotIndex) const {
            return depotIndex + customersCount;
        }

    private:
        void create(const string& datasetFilename);

        void loadCordeauInstance(const string& filename);

        void loadStodolaInstance(const string& filename);
        int loadGeneralInfo(const string& key, const string& value);
        int loadObjectInfo(const string& key, const string& value);
        int loadDepotInfo(const string& info, const string& value, int depotIndex);
        int loadCustomerInfo(const string& info, const string& value, int customerIndex);

        void createDistanceMatrices();
};

#endif