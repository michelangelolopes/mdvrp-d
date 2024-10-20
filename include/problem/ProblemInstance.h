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
        ProblemInstance(string datasetFilename) {
            create(datasetFilename);
        }

        string name;
        
        Depot* depots = nullptr;
        int depotsCount;
        
        Customer* customers = nullptr;
        int customersCount;

        double** depotToCustomerDistanceMatrix = nullptr;
        double** customerToCustomerDistanceMatrix = nullptr;

        void create(string datasetFilename);
        void finalize();
        void print(int printDistanceMatrix);

        int loadGeneralInfo(string key, string value);
        int loadObjectInfo(string key, string value);
        int loadDepotInfo(string object, string info, string value);
        int loadCustomerInfo(string object, string info, string value);

        void createDistanceMatrix();
};

int extractIndex(istringstream& stream);

#endif