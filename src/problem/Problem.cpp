#include "../../include/problem/Problem.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "../../include/utils/ArrayUtils.h"
#include "../../include/utils/MathUtils.h"

void Problem::create(string filename) {

    // Problem problem;

    ifstream file;
    file.open(filename);

    string line;
    while(std::getline(file, line)) {
        int delimiterIndex = line.find('=');

        if(delimiterIndex != -1) {
            string key = line.substr(0, delimiterIndex - 1);
            string value = line.substr(delimiterIndex + 2, line.length());
            // std::cout << key << '\n' << value << '\n';
            int isGeneralInfo = loadGeneralInfo(key, value);
            int isObjectInfo = !isGeneralInfo && loadObjectInfo(key, value);

        }
    }
    
    #ifndef NDEBUG
    print();
    #endif

    // return problem;
    // fscanf(file, "Problem = %s", name);
}

void Problem::finalize() {
    if(this->customers != nullptr) {
        free(this->customers);
    }

    if(this->depots != nullptr) {
        for(int depotIndex = 0; depotIndex < this->depotCount; depotIndex++) {
            this->depots[depotIndex].finalize();
        }
        free(this->depots);
    }

    if(this->customerDistanceMatrix != nullptr) {
        freeMatrix((void**)this->customerDistanceMatrix, this->customerCount);
    }

    if(this->depotDistanceMatrix != nullptr) {
        freeMatrix((void**)this->depotDistanceMatrix, this->depotCount);
    }
}

void Problem::print() {

    std::cout << "----------------------------\n";
    std::cout << name << '\n';
    std::cout << depotCount << '\n';
    std::cout << customerCount << '\n';

    for(int depotIndex = 0; depotIndex < depotCount; depotIndex++) {
        std::cout << "----------------------------\n";
        std::cout << "Depot " << depotIndex + 1 << '\n';
        std::cout << "x: " << depots[depotIndex].x << '\n';
        std::cout << "y: " << depots[depotIndex].y << '\n';
        std::cout << "truck.speed: " << depots[depotIndex].trucks[0].speed << '\n';
        std::cout << "truck.capacity: " << depots[depotIndex].trucks[0].capacity << '\n';
        std::cout << "truck.serviceTime: " << depots[depotIndex].trucks[0].serviceTime << '\n';
        std::cout << "truck.routeMaxDuration: " << depots[depotIndex].trucks[0].routeMaxDuration << '\n';
        std::cout << "uav.speed: " << depots[depotIndex].uavs[0].speed << '\n';
        std::cout << "uav.capacity: " << depots[depotIndex].uavs[0].capacity << '\n';
        std::cout << "uav.serviceTime: " << depots[depotIndex].uavs[0].serviceTime << '\n';
        std::cout << "uav.endurance: " << depots[depotIndex].uavs[0].endurance << '\n';
        std::cout << "uav.launchTime: " << depots[depotIndex].uavs[0].launchTime << '\n';
        std::cout << "uav.recoveryTime: " << depots[depotIndex].uavs[0].recoveryTime << '\n';
    }

    std::cout << "----------------------------\n";

    for(int customerIndex = 0; customerIndex < customerCount; customerIndex++) {
        std::cout << "----------------------------\n";
        std::cout << "Customer " << customerIndex + 1 << '\n';
        std::cout << "x: " << customers[customerIndex].x << '\n';
        std::cout << "y: " << customers[customerIndex].y << '\n';
        std::cout << "demand: " << customers[customerIndex].demand << '\n';
    }

    std::cout << "----------------------------\n";
}

int Problem::loadGeneralInfo(string key, string value) {
    // int isProblem = ;
    // int isDepotCount = ;
    // int isCustomerCount = ;
    // int isVehicleInfo = !isCustomerCount && key.compare("Depot");

    
    // std::cout << "Depot: " << key.compare("Depot") << '\n';

    // std::cout << "isProblem: " << isProblem << '\n';
    // std::cout << "isDepotCount: " << isDepotCount << '\n';
    // std::cout << "isCustomerCount: " << isCustomerCount << '\n';

    istringstream valueStream;
    valueStream.str(value);
    
    if(key.compare("Problem") == 0) {
        valueStream >> name;
        return 1;
    } 
    
    if(key.compare("NumberOfDepots") == 0) {
        valueStream >> depotCount;
        depots = (Depot*) initialize(depotCount, sizeof(Depot));
        return 1;
    } 
    
    if(key.compare("NumberOfNodes") == 0) {
        valueStream >> customerCount;
        customers = (Customer*) initialize(customerCount, sizeof(Customer));
        return 1;
    }

    return 0;
}

int Problem::loadObjectInfo(string key, string value) {

    int delimiterIndex = key.find('_');
    string object = key.substr(0, delimiterIndex);
    string info = key.substr(delimiterIndex + 1, key.length());
    
    // std::cout << object << '\n';
    // std::cout << info << '\n';

    int vehicleComparison = object.compare("Depot");
    // std::cout << vehicleComparison << '\n';
    int isVehicleInfo = vehicleComparison >= 0 && vehicleComparison <= 3;
    // std::cout << "isVehicleInfo" << isVehicleInfo << '\n';

    if(isVehicleInfo) {
        loadVehicleInfo(object, info, value);
        return 1;
    }

    int customerComparison = object.compare("Node");
    // std::cout << customerComparison << '\n';
    int isCustomerInfo = customerComparison >= 0 && customerComparison <= 3;

    if(isCustomerInfo) {
        loadCustomerInfo(object, info, value);
        return 1;
    }

    
    // std::cout << "object: " << object << '\n';
    // std::cout << "object compare: " << object.compare("Node") << '\n';
    
    // istringstream keyStream;
    // keyStream.str(key);


    return 0;
}

int Problem::loadVehicleInfo(string object, string info, string value) {

    istringstream objectStream;
    objectStream.str(object);

    string tempString;
    int depotIndex = extractIndex(objectStream);
    depotIndex--;

    if(depots[depotIndex].trucks == NULL) {
        depots[depotIndex].trucks = (Truck*) initialize(vehicleCount, sizeof(Truck));
        depots[depotIndex].uavs = (Uav*) initialize(vehicleCount, sizeof(Uav));
    }

    // std::cout << object << '\n';

    istringstream valueStream;
    valueStream.str(value);

    if(info.compare("TruckVelocity") == 0) {
        // std::cout << object << '\n';
        // std::cout << info << '\n';
        // std::cout << value << '\n';
        valueStream >> depots[depotIndex].trucks[0].speed;
        // std::cout << "trucks[0].speed: " << depots[depotIndex].trucks[0].speed << '\n';
        return 1;
    }

    if(info.compare("DroneVelocity") == 0) {
        valueStream >> depots[depotIndex].uavs[0].speed;
        return 1;
    }

    if(info.compare("TruckCapacity") == 0) {
        valueStream >> depots[depotIndex].trucks[0].capacity;
        return 1;
    }

    if(info.compare("DroneCapacity") == 0) {
        valueStream >> depots[depotIndex].uavs[0].capacity;
        return 1;
    }

    if(info.compare("RouteMaxDuration") == 0) {
        valueStream >> depots[depotIndex].trucks[0].routeMaxDuration;
        return 1;
    }
    
    if(info.compare("DroneEndurance") == 0) {
        valueStream >> depots[depotIndex].uavs[0].endurance;
        return 1;
    }

    if(info.compare("TruckServiceTime") == 0) {
        valueStream >> depots[depotIndex].trucks[0].serviceTime;
        return 1;
    }

    if(info.compare("DroneServiceTime") == 0) {
        valueStream >> depots[depotIndex].uavs[0].serviceTime;
        return 1;
    }

    if(info.compare("DroneLaunchTime") == 0) {
        valueStream >> depots[depotIndex].uavs[0].launchTime;
        return 1;
    }

    if(info.compare("DroneRecoveryTime") == 0) {
        valueStream >> depots[depotIndex].uavs[0].recoveryTime;
        return 1;
    }

    if(info.compare("Position") == 0) {
        char temp;
        valueStream >> depots[depotIndex].x >> temp >> depots[depotIndex].y;
        // std::cout << "x: " << depots[depotIndex].x << '\n';
        // std::cout << "y: " << depots[depotIndex].y << '\n';
        return 1;
    }

    return 0;
}

int Problem::loadCustomerInfo(string object, string info, string value) {

    istringstream objectStream;
    objectStream.str(object);

    string tempString;
    int customerIndex = extractIndex(objectStream);
    customerIndex--;

    // std::cout << object << '\n';

    istringstream valueStream;
    valueStream.str(value);

    if(info.compare("Demand") == 0) {
        // std::cout << object << '\n';
        // std::cout << info << '\n';
        // std::cout << value << '\n';
        valueStream >> customers[customerIndex].demand;
        // std::cout << "trucks[0].speed: " << depots[depotIndex].trucks[0].speed << '\n';
        return 1;
    }

    if(info.compare("Position") == 0) {
        char temp;
        valueStream >> customers[customerIndex].x >> temp >> customers[customerIndex].y;
        // std::cout << "x: " << depots[depotIndex].x << '\n';
        // std::cout << "y: " << depots[depotIndex].y << '\n';
        return 1;
    }

    return 0;
}

void Problem::initializeDistanceMatrix() {
    customerDistanceMatrix = (double**) initialize(customerCount, sizeof(double*), sizeof(double));

    for(int customerIndex = 0; customerIndex < customerCount; customerIndex++) {
        for(int neighborIndex = 0; neighborIndex < customerCount; neighborIndex++) {
            customerDistanceMatrix[customerIndex][neighborIndex] = calculateEuclidianDistance(
                customers[customerIndex],
                customers[neighborIndex]
            );
        }
    }

    #ifndef NDEBUG
    printDistanceMatrix();
    #endif
}

void Problem::initializeDepotDistanceMatrix() {
    depotDistanceMatrix = (double**) initialize(depotCount, customerCount, sizeof(double*), sizeof(double));

    for(int depotIndex = 0; depotIndex < depotCount; depotIndex++) {
        for(int customerIndex = 0; customerIndex < customerCount; customerIndex++) {
            customerDistanceMatrix[depotIndex][customerIndex] = calculateEuclidianDistance(
                depots[depotIndex],
                customers[customerIndex]
            );
        }
    }
}

void Problem::printDistanceMatrix() {
    for(int customerIndex = 0; customerIndex < customerCount; customerIndex++) {
        for(int neighborIndex = 0; neighborIndex < customerCount; neighborIndex++) {
            std::cout << "[" << customerIndex << "]" << "[" << neighborIndex << "] = " << customerDistanceMatrix[customerIndex][neighborIndex] << '\n';
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