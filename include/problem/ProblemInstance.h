#ifndef PROBLEM_INSTANCE_H
#define PROBLEM_INSTANCE_H

// #include <cstddef>
// #include "../utils/FileUtils.h"
#include <fstream>
#include <string>

#include "Customer.h"
#include "Depot.h"

using namespace std;

class ProblemInstance {
    public:
        ProblemInstance() {}

        ProblemInstance(string datasetFilename) {
            create(datasetFilename);
            initializeDistanceMatrix();
        }

        string name;
        
        Depot* depots = nullptr;
        int depotCount;
        int vehicleCount = 1;
        
        Customer* customers = nullptr;
        int customerCount;

        int vertexCount;
        double** distanceMatrix = nullptr;

        void initializeDistanceMatrix();
        void printDistanceMatrix();

        void create(string datasetFilename);
        void print();
        void finalize();

        int loadGeneralInfo(string key, string value);
        int loadObjectInfo(string key, string value);
        int loadVehicleInfo(string object, string info, string value);
        int loadCustomerInfo(string object, string info, string value);
};

int extractIndex(istringstream& stream);

#endif