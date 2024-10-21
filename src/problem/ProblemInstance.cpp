#include "../../include/problem/ProblemInstance.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "../../include/utils/ArrayUtils.h"
#include "../../include/utils/MathUtils.h"

void ProblemInstance::create(string filename) {

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

    createDistanceMatrix();
}

void ProblemInstance::finalize() {
    if(this->customers != nullptr) {
        free(this->customers);
    }

    if(this->depots != nullptr) {
        free(this->depots);
    }

    if(this->customerToCustomerDistanceMatrix != nullptr) {
        freeMatrix(this->customerToCustomerDistanceMatrix, this->customersCount);
    }

    if(this->depotToCustomerDistanceMatrix != nullptr) {
        freeMatrix(this->depotToCustomerDistanceMatrix, this->depotsCount);
    }
}

void ProblemInstance::print(int printDistanceMatrix) {

    std::cout << "\n--------------------------------------------------\n";

    std::cout << name << " - ";
    std::cout << "depotsCount: " << depotsCount << " - ";
    std::cout << "customersCount: " << customersCount << "\n";

    std::cout << "-------------------------\n";
    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        std::cout << "Depot[" << depotIndex << "] - ";
        depots[depotIndex].print();

        if(depotIndex < depotsCount - 1) {
            std::cout << "------------\n";
        }
    }

    std::cout << "-------------------------\n";
    for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
        std::cout << "Customer[" << customerIndex << "] - ";
        customers[customerIndex].print();

        if(customerIndex < customersCount - 1) {
            std::cout << "------------\n";
        }
    }

    if(printDistanceMatrix) {
        std::cout << "-------------------------\n";
        for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
            for(int neighborCustomerIndex = 0; neighborCustomerIndex < customersCount; neighborCustomerIndex++) {
                std::cout << "Distance Customer-Customer: ";
                std::cout << "[" << customerIndex << "]";
                std::cout << "[" << neighborCustomerIndex << "]";
                std::cout << " = " << customerToCustomerDistanceMatrix[customerIndex][neighborCustomerIndex] << '\n';
            }
        }

        std::cout << "-------------------------\n";
        for(int depotIndex = customersCount; depotIndex < depotsCount; depotIndex++) {
            for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
                std::cout << "Distance Depot-Customer: ";
                std::cout << "[" << depotIndex << "]";
                std::cout << "[" << customerIndex << "]";
                std::cout << " = " << depotToCustomerDistanceMatrix[depotIndex][customerIndex] << '\n';
            }
        }
    }
}

int ProblemInstance::loadGeneralInfo(string key, string value) {

    istringstream valueStream;
    valueStream.str(value);
    
    if(key.compare("Problem") == 0) {
        valueStream >> name;
        return 1;
    } 
    
    if(key.compare("NumberOfDepots") == 0) {
        valueStream >> depotsCount;
        depots = (Depot*) calloc(depotsCount, sizeof(Depot));
        return 1;
    } 
    
    if(key.compare("NumberOfNodes") == 0) {
        valueStream >> customersCount;
        customers = (Customer*) calloc(customersCount, sizeof(Customer));
        return 1;
    }

    return 0;
}

int ProblemInstance::loadObjectInfo(string key, string value) {

    int delimiterIndex = key.find('_');
    string object = key.substr(0, delimiterIndex);
    string info = key.substr(delimiterIndex + 1, key.length());

    int depotComparison = object.compare("Depot");
    
    int isDepotInfo = depotComparison >= 0 && depotComparison <= 3;
    if(isDepotInfo) {
        loadDepotInfo(object, info, value);
        return 1;
    }

    int customerComparison = object.compare("Node");

    int isCustomerInfo = customerComparison >= 0 && customerComparison <= 3;
    if(isCustomerInfo) {
        loadCustomerInfo(object, info, value);
        return 1;
    }

    return 0;
}

int ProblemInstance::loadDepotInfo(string object, string info, string value) {

    istringstream objectStream;
    objectStream.str(object);

    string tempString;
    int depotIndex = extractIndex(objectStream);
    depotIndex--;

    istringstream valueStream;
    valueStream.str(value);

    if(info.compare("TruckVelocity") == 0) {
        valueStream >> depots[depotIndex].truck.speed;
        return 1;
    }

    if(info.compare("DroneVelocity") == 0) {
        valueStream >> depots[depotIndex].uav.speed;
        return 1;
    }

    if(info.compare("TruckCapacity") == 0) {
        valueStream >> depots[depotIndex].truck.capacity;
        return 1;
    }

    if(info.compare("DroneCapacity") == 0) {
        valueStream >> depots[depotIndex].uav.capacity;
        return 1;
    }

    if(info.compare("RouteMaxDuration") == 0) {
        valueStream >> depots[depotIndex].truck.routeMaxDuration;
        return 1;
    }
    
    if(info.compare("DroneEndurance") == 0) {
        valueStream >> depots[depotIndex].uav.endurance;
        return 1;
    }

    if(info.compare("TruckServiceTime") == 0) {
        valueStream >> depots[depotIndex].truck.serviceTime;
        return 1;
    }

    if(info.compare("DroneServiceTime") == 0) {
        valueStream >> depots[depotIndex].uav.serviceTime;
        return 1;
    }

    if(info.compare("DroneLaunchTime") == 0) {
        valueStream >> depots[depotIndex].uav.launchTime;
        return 1;
    }

    if(info.compare("DroneRecoveryTime") == 0) {
        valueStream >> depots[depotIndex].uav.recoveryTime;
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

int ProblemInstance::loadCustomerInfo(string object, string info, string value) {

    istringstream objectStream;
    objectStream.str(object);

    string tempString;
    int customerIndex = extractIndex(objectStream);
    customerIndex--;

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

void ProblemInstance::createDistanceMatrix() {

    this->customerToCustomerDistanceMatrix = (double**) callocMatrix(customersCount, sizeof(double*), sizeof(double));
    for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
        for(int neighborCustomerIndex = 0; neighborCustomerIndex < customersCount; neighborCustomerIndex++) {
            customerToCustomerDistanceMatrix[customerIndex][neighborCustomerIndex] = calculateEuclidianDistance(
                customers[customerIndex].position,
                customers[neighborCustomerIndex].position
            );
        }
    }

    this->depotToCustomerDistanceMatrix = (double**) callocMatrix(depotsCount, customersCount, sizeof(double*), sizeof(double));
    for(int depotIndex = 0; depotIndex < depotsCount; depotIndex++) {
        for(int customerIndex = 0; customerIndex < customersCount; customerIndex++) {
            depotToCustomerDistanceMatrix[depotIndex][customerIndex] = calculateEuclidianDistance(
                depots[depotIndex].position,
                customers[customerIndex].position
            );
        }
    }
}

int extractIndex(istringstream& stream) {
    char temp;
    int value;
    while (stream >> temp) {
        if (isdigit(temp)) {
            stream.putback(temp);
            break;
        }
    }

    stream >> value;

    return value;
}