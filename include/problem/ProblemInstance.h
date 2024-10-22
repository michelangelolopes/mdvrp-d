#ifndef PROBLEM_INSTANCE_H
#define PROBLEM_INSTANCE_H

#include <fstream>
#include <string>

#include "../enum/MinimizationTypeEnum.h"
#include "../enum/ProblemTypeEnum.h"

#include "Customer.h"
#include "Depot.h"

using namespace std;

class ProblemInstance {
    public:
        ProblemInstance(string datasetFilename, ProblemType problemType) : 
        problemType(problemType)
        {
            create(datasetFilename);
        }

        ProblemType problemType;
        MinimizationType minimizationType;
        
        Depot* depots = nullptr;
        int depotsCount;
        
        Customer* customers = nullptr;
        int customersCount;

        double** depotToCustomerDistanceMatrix = nullptr;
        double** customerToCustomerDistanceMatrix = nullptr;

        void create(string datasetFilename);
        void finalize();
        void print(int printDistanceMatrix);

    private:
        void loadCordeauInstance(string filename);

        void loadStodolaInstance(string filename);
        int loadGeneralInfo(string key, string value);
        int loadObjectInfo(string key, string value);
        int loadDepotInfo(string object, string info, string value);
        int loadCustomerInfo(string object, string info, string value);

        void createDistanceMatrices();
};

int extractIndex(istringstream& stream);

#endif