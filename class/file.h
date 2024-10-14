#include <fstream>
#include <iostream>
#include <sstream>
// #include <string>

// #include "customer.h"
// #include "depot.h"
#include "array.h"

using namespace std;
#include "problem.h"

int loadGeneralInfo(string key, string value, ProblemParameters& problem) {
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
        valueStream >> problem.name;
        return 1;
    } 
    
    if(key.compare("NumberOfDepots") == 0) {
        valueStream >> problem.depotCount;
        problem.depots = (Depot*) initialize(problem.depotCount, sizeof(Depot));
        return 1;
    } 
    
    if(key.compare("NumberOfNodes") == 0) {
        valueStream >> problem.customerCount;
        problem.customers = (Customer*) initialize(problem.customerCount, sizeof(Customer));
        return 1;
    }

    return 0;
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

int loadVehicleInfo(string object, string info, string value, ProblemParameters& problem) {

    istringstream objectStream;
    objectStream.str(object);

    string tempString;
    int depotIndex = extractIndex(objectStream);
    depotIndex--;

    if(problem.depots[depotIndex].trucks == NULL) {
        problem.depots[depotIndex].trucks = (Truck*) initialize(problem.vehicleCount, sizeof(Truck));
        problem.depots[depotIndex].uavs = (Uav*) initialize(problem.vehicleCount, sizeof(Uav));
    }

    // std::cout << object << '\n';

    istringstream valueStream;
    valueStream.str(value);

    if(info.compare("TruckVelocity") == 0) {
        // std::cout << object << '\n';
        // std::cout << info << '\n';
        // std::cout << value << '\n';
        valueStream >> problem.depots[depotIndex].trucks[0].speed;
        // std::cout << "trucks[0].speed: " << problem.depots[depotIndex].trucks[0].speed << '\n';
        return 1;
    }

    if(info.compare("DroneVelocity") == 0) {
        valueStream >> problem.depots[depotIndex].uavs[0].speed;
        return 1;
    }

    if(info.compare("TruckCapacity") == 0) {
        valueStream >> problem.depots[depotIndex].trucks[0].capacity;
        return 1;
    }

    if(info.compare("DroneCapacity") == 0) {
        valueStream >> problem.depots[depotIndex].uavs[0].capacity;
        return 1;
    }

    if(info.compare("RouteMaxDuration") == 0) {
        valueStream >> problem.depots[depotIndex].trucks[0].routeMaxDuration;
        return 1;
    }
    
    if(info.compare("DroneEndurance") == 0) {
        valueStream >> problem.depots[depotIndex].uavs[0].endurance;
        return 1;
    }

    if(info.compare("TruckServiceTime") == 0) {
        valueStream >> problem.depots[depotIndex].trucks[0].serviceTime;
        return 1;
    }

    if(info.compare("DroneServiceTime") == 0) {
        valueStream >> problem.depots[depotIndex].uavs[0].serviceTime;
        return 1;
    }

    if(info.compare("DroneLaunchTime") == 0) {
        valueStream >> problem.depots[depotIndex].uavs[0].launchTime;
        return 1;
    }

    if(info.compare("DroneRecoveryTime") == 0) {
        valueStream >> problem.depots[depotIndex].uavs[0].recoveryTime;
        return 1;
    }

    if(info.compare("Position") == 0) {
        char temp;
        valueStream >> problem.depots[depotIndex].x >> temp >> problem.depots[depotIndex].y;
        // std::cout << "x: " << problem.depots[depotIndex].x << '\n';
        // std::cout << "y: " << problem.depots[depotIndex].y << '\n';
        return 1;
    }

    return 0;
}

int loadCustomerInfo(string object, string info, string value, ProblemParameters& problem) {

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
        valueStream >> problem.customers[customerIndex].demand;
        // std::cout << "trucks[0].speed: " << problem.depots[depotIndex].trucks[0].speed << '\n';
        return 1;
    }

    if(info.compare("Position") == 0) {
        char temp;
        valueStream >> problem.customers[customerIndex].x >> temp >> problem.customers[customerIndex].y;
        // std::cout << "x: " << problem.depots[depotIndex].x << '\n';
        // std::cout << "y: " << problem.depots[depotIndex].y << '\n';
        return 1;
    }

    return 0;
}

int loadObjectInfo(string key, string value, ProblemParameters& problem) {

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
        loadVehicleInfo(object, info, value, problem);
        return 1;
    }

    int customerComparison = object.compare("Node");
    // std::cout << customerComparison << '\n';
    int isCustomerInfo = customerComparison >= 0 && customerComparison <= 3;

    if(isCustomerInfo) {
        loadCustomerInfo(object, info, value, problem);
        return 1;
    }

    
    // std::cout << "object: " << object << '\n';
    // std::cout << "object compare: " << object.compare("Node") << '\n';
    
    // istringstream keyStream;
    // keyStream.str(key);


    return 0;
}


void printProblem(ProblemParameters& problem) {

    std::cout << "----------------------------\n";
    std::cout << problem.name << '\n';
    std::cout << problem.depotCount << '\n';
    std::cout << problem.customerCount << '\n';

    for(int depotIndex = 0; depotIndex < problem.depotCount; depotIndex++) {
        std::cout << "----------------------------\n";
        std::cout << "Depot " << depotIndex + 1 << '\n';
        std::cout << "x: " << problem.depots[depotIndex].x << '\n';
        std::cout << "y: " << problem.depots[depotIndex].y << '\n';
        std::cout << "truck.speed: " << problem.depots[depotIndex].trucks[0].speed << '\n';
        std::cout << "truck.capacity: " << problem.depots[depotIndex].trucks[0].capacity << '\n';
        std::cout << "truck.serviceTime: " << problem.depots[depotIndex].trucks[0].serviceTime << '\n';
        std::cout << "truck.routeMaxDuration: " << problem.depots[depotIndex].trucks[0].routeMaxDuration << '\n';
        std::cout << "uav.speed: " << problem.depots[depotIndex].uavs[0].speed << '\n';
        std::cout << "uav.capacity: " << problem.depots[depotIndex].uavs[0].capacity << '\n';
        std::cout << "uav.serviceTime: " << problem.depots[depotIndex].uavs[0].serviceTime << '\n';
        std::cout << "uav.endurance: " << problem.depots[depotIndex].uavs[0].endurance << '\n';
        std::cout << "uav.launchTime: " << problem.depots[depotIndex].uavs[0].launchTime << '\n';
        std::cout << "uav.recoveryTime: " << problem.depots[depotIndex].uavs[0].recoveryTime << '\n';
    }

    std::cout << "----------------------------\n";

    for(int customerIndex = 0; customerIndex < problem.customerCount; customerIndex++) {
        std::cout << "----------------------------\n";
        std::cout << "Customer " << customerIndex + 1 << '\n';
        std::cout << "x: " << problem.customers[customerIndex].x << '\n';
        std::cout << "y: " << problem.customers[customerIndex].y << '\n';
        std::cout << "demand: " << problem.customers[customerIndex].demand << '\n';
    }

    std::cout << "----------------------------\n";
}

void loadFile(char const* filename, ProblemParameters& problem) {

    ifstream file;
    file.open(filename);

    string line;
    while(std::getline(file, line)) {
        int delimiterIndex = line.find('=');

        if(delimiterIndex != -1) {
            string key = line.substr(0, delimiterIndex - 1);
            string value = line.substr(delimiterIndex + 2, line.length());
            // std::cout << key << '\n' << value << '\n';
            int isGeneralInfo = loadGeneralInfo(key, value, problem);
            int isObjectInfo = !isGeneralInfo && loadObjectInfo(key, value, problem);

        }
    }
    
    // printProblem(problem);


    // fscanf(file, "Problem = %s", problem.name);
}