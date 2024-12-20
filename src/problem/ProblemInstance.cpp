#include "../../include/problem/ProblemInstance.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "../../include/enum/MinimizationTypeEnum.hpp"
#include "../../include/utils/ArrayUtils.hpp"
#include "../../include/utils/MathUtils.hpp"

void ProblemInstance::create(const string& filename) {

    switch(problemType) {
        case VRP:
            std::cout << "VRP\n";
            break;
        case MDVRP:
            loadCordeauInstance(filename);
            minimizationType = TOTAL_DISTANCE_TRAVELED;
            break;
        case VRP_D:
            std::cout << "VRP-D\n";
            break;
        case MDVRP_D:
            loadStodolaInstance(filename);
            minimizationType = MAX_TIME_SPENT;
            break;                                
    }

    createDistanceMatrices();
}

void ProblemInstance::finalize() {
    if(customers != nullptr) {
        free(customers);
    }

    if(depots != nullptr) {
        free(depots);
    }

    if(verticesDistanceMatrix != nullptr) {
        freeMatrix(verticesDistanceMatrix, verticesCount);
    }
}

void ProblemInstance::print(int printDistanceMatrix) const {

    std::cout << "\n--------------------------------------------------\n";

    switch(problemType) {
        case VRP:
            std::cout << "VRP - ";
            break;
        case MDVRP:
            std::cout << "MDVRP - ";
            break;
        case VRP_D:
            std::cout << "VRP-D - ";
            break;
        case MDVRP_D:
            std::cout << "MDVRP-D - ";
            break;                                
    }
    
    std::cout << "depotsCount: " << depotsCount << " - ";
    std::cout << "customersCount: " << customersCount << "\n";

    std::cout << "-------------------------\n";
    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        std::cout << "Depot[" << depotIndex << "] - ";
        depots[depotIndex].print(problemType);

        if(depotIndex < depotsCount - 1) {
            std::cout << "------------\n";
        }
    }

    std::cout << "-------------------------\n";
    for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
        std::cout << "Customer[" << customerIndex << "] - ";
        customers[customerIndex].print();
    }

    if(printDistanceMatrix) {
        std::cout << "-------------------------\n";
        for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
            for(int neighborCustomerIndex = 0; neighborCustomerIndex < customersCount; neighborCustomerIndex++) {
                std::cout << "Distance Customer-Customer: ";
                std::cout << "[" << customerIndex << "]";
                std::cout << "[" << neighborCustomerIndex << "]";
                std::cout << " = " << verticesDistanceMatrix[customerIndex][neighborCustomerIndex] << '\n';
            }
        }

        std::cout << "-------------------------\n";
        for(int depotIndex = customersCount; depotIndex < depotsCount; depotIndex++) {
            for(int neighborCustomerIndex = 0; neighborCustomerIndex < customersCount; neighborCustomerIndex++) {
                std::cout << "Distance Depot-Customer: ";
                std::cout << "[" << depotIndex << "]";
                std::cout << "[" << neighborCustomerIndex << "]";

                int depotVertexIndex = getDepotVertexIndex(depotIndex);
                std::cout << " = " << verticesDistanceMatrix[depotVertexIndex][neighborCustomerIndex] << '\n';
            }
        }
    }
}

void ProblemInstance::loadCordeauInstance(const string& filename) {

    ifstream file;
    file.open(filename);

    string line;
    std::getline(file, line);
    
    istringstream valueStream(line);

    int temp;
    valueStream >> temp; //problem type
    valueStream >> temp; //vehicle count
    valueStream >> customersCount;
    valueStream >> depotsCount;

    depots = (Depot*) malloc(depotsCount * sizeof(Depot));
    customers = (Customer*) malloc(customersCount * sizeof(Customer));

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        std::getline(file, line);
        valueStream.str(line);

        valueStream >> depots[depotIndex].truck.routeMaxDuration;
        valueStream >> depots[depotIndex].truck.capacity;
        depots[depotIndex].truck.speed = 0;
        depots[depotIndex].truck.serviceTime = 0;
    }

    for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
        std::getline(file, line);
        valueStream.str(line);

        valueStream >> temp; //customer number

        int x;
        int y;
        valueStream >> x >> y;
        customers[customerIndex].position = Position2D(x, y);

        valueStream >> temp; //service duration
        valueStream >> customers[customerIndex].demand;

        valueStream >> temp; //frequency of visit
        
        int visitCombinationsCount;
        valueStream >> visitCombinationsCount; //number of visit combinations

        for(int i = 0; i < visitCombinationsCount; i++) {
            valueStream >> temp;
        }
    }

    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        std::getline(file, line);
        valueStream.str(line);

        valueStream >> temp; //depot number

        int x;
        int y;
        valueStream >> x >> y;
        depots[depotIndex].position = Position2D(x, y);

        valueStream >> temp >> temp >> temp >> temp;
    }
}

void ProblemInstance::loadStodolaInstance(const string& filename) {

    ifstream file;
    file.open(filename);

    string line;
    while(std::getline(file, line)) {
        int delimiterIndex = line.find('=');

        if(delimiterIndex != -1) {
            string key = line.substr(0, delimiterIndex - 1);
            string value = line.substr(delimiterIndex + 2, line.length());

            if(!loadGeneralInfo(key, value)) {
                loadObjectInfo(key, value);
            }
        }
    }
}

int ProblemInstance::loadGeneralInfo(const string& key, const string& value) {

    istringstream valueStream;
    valueStream.str(value);

    if(key.compare("Problem") == 0) {
        return 1; //don´t need to store this
    }
    
    if(key.compare("NumberOfDepots") == 0) {
        valueStream >> depotsCount;
        depots = (Depot*) malloc(depotsCount * sizeof(Depot));
        return 1;
    } 
    
    if(key.compare("NumberOfNodes") == 0) {
        valueStream >> customersCount;
        customers = (Customer*) malloc(customersCount * sizeof(Customer));
        return 1;
    }

    return 0;
}

int ProblemInstance::loadObjectInfo(const string& key, const string& value) {

    int delimiterIndex = key.find('_');
    string object = key.substr(0, delimiterIndex);
    string info = key.substr(delimiterIndex + 1, key.length());

    int depotComparison = object.compare("Depot");
    
    int isDepotInfo = depotComparison >= 0 && depotComparison <= 3;
    if(isDepotInfo) {

        string depotIndexStr = object.substr(object.length() - depotComparison);
        int depotIndex = std::stoi(depotIndexStr);
        
        loadDepotInfo(info, value, --depotIndex);
        
        return 1;
    }

    int customerComparison = object.compare("Node");

    int isCustomerInfo = customerComparison >= 0 && customerComparison <= 3;
    if(isCustomerInfo) {

        string customerIndexStr = object.substr(object.length() - customerComparison);
        int customerIndex = std::stoi(customerIndexStr);

        loadCustomerInfo(info, value, --customerIndex);

        return 1;
    }

    return 0;
}

int ProblemInstance::loadDepotInfo(const string& info, const string& value, int depotIndex) {

    istringstream valueStream;
    valueStream.str(value);

    if(info.compare("TruckVelocity") == 0) {
        valueStream >> depots[depotIndex].truck.speed;
        return 1;
    }

    if(info.compare("DroneVelocity") == 0) {
        valueStream >> depots[depotIndex].drone.speed;
        return 1;
    }

    if(info.compare("TruckCapacity") == 0) {
        valueStream >> depots[depotIndex].truck.capacity;
        return 1;
    }

    if(info.compare("DroneCapacity") == 0) {
        valueStream >> depots[depotIndex].drone.capacity;
        return 1;
    }

    if(info.compare("RouteMaxDuration") == 0) {
        valueStream >> depots[depotIndex].truck.routeMaxDuration;
        return 1;
    }
    
    if(info.compare("DroneEndurance") == 0) {
        valueStream >> depots[depotIndex].drone.endurance;
        return 1;
    }

    if(info.compare("TruckServiceTime") == 0) {
        valueStream >> depots[depotIndex].truck.serviceTime;
        return 1;
    }

    if(info.compare("DroneServiceTime") == 0) {
        valueStream >> depots[depotIndex].drone.serviceTime;
        return 1;
    }

    if(info.compare("DroneLaunchTime") == 0) {
        valueStream >> depots[depotIndex].drone.launchTime;
        return 1;
    }

    if(info.compare("DroneRecoveryTime") == 0) {
        valueStream >> depots[depotIndex].drone.recoveryTime;
        return 1;
    }

    if(info.compare("Position") == 0) {
        char temp;
        double x;
        double y;
        valueStream >> x >> temp >> y;
        depots[depotIndex].position = Position2D(x, y);
        return 1;
    }

    return 0;
}

int ProblemInstance::loadCustomerInfo(const string& info, const string& value, int customerIndex) {

    istringstream valueStream;
    valueStream.str(value);

    if(info.compare("Demand") == 0) {
        valueStream >> customers[customerIndex].demand;
        return 1;
    }

    if(info.compare("Position") == 0) {
        char temp;
        double x;
        double y;
        valueStream >> x >> temp >> y;
        customers[customerIndex].position = Position2D(x, y);
        return 1;
    }

    return 0;
}

void ProblemInstance::createDistanceMatrices() {

    verticesCount = customersCount + depotsCount;
    verticesDistanceMatrix = (double**) mallocMatrix(verticesCount, verticesCount, sizeof(double*), sizeof(double));

    for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
        
        verticesDistanceMatrix[customerIndex][customerIndex] = 0;
        for(int neighborCustomerIndex = customerIndex + 1; neighborCustomerIndex < customersCount; neighborCustomerIndex++) {
            
            verticesDistanceMatrix[customerIndex][neighborCustomerIndex] = calculateEuclidianDistance(
                customers[customerIndex].position,
                customers[neighborCustomerIndex].position
            );

            verticesDistanceMatrix[neighborCustomerIndex][customerIndex] = verticesDistanceMatrix[customerIndex][neighborCustomerIndex];
        }
    }

    //do not need to fill depot-depot distances, because this kind of movement will not happen
    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        for(int neighborCustomerIndex = 0; neighborCustomerIndex < customersCount; neighborCustomerIndex++) {

            int depotVertexIndex = getDepotVertexIndex(depotIndex);
            verticesDistanceMatrix[depotVertexIndex][neighborCustomerIndex] = calculateEuclidianDistance(
                depots[depotIndex].position,
                customers[neighborCustomerIndex].position
            );

            verticesDistanceMatrix[neighborCustomerIndex][depotVertexIndex] = verticesDistanceMatrix[depotVertexIndex][neighborCustomerIndex];
        }
    }
}