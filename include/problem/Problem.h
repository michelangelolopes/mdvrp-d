#ifndef PROBLEM_H
#define PROBLEM_H

// #include <cstddef>
// #include "../utils/FileUtils.h"
#include <fstream>
#include <string>

#include "Customer.h"
#include "Depot.h"

using namespace std;

class Problem {
    public:
        Problem() {}

        Problem(string datasetFilename) {
            create(datasetFilename);
            initializeDistanceMatrix();
        }

        string name;
        
        Depot* depots = nullptr;
        int depotCount;
        int vehicleCount = 1;
        
        Customer* customers = nullptr;
        int customerCount;
        double** customerDistanceMatrix = nullptr;

    void create(string datasetFilename);
    void print();

    int loadGeneralInfo(string key, string value);
    int loadObjectInfo(string key, string value);
    int loadVehicleInfo(string object, string info, string value);
    int loadCustomerInfo(string object, string info, string value);

    void initializeDistanceMatrix();
    void printDistanceMatrix();
};

int extractIndex(istringstream& stream);

#endif