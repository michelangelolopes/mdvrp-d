#ifndef PROBLEM_INSTANCE_HPP
#define PROBLEM_INSTANCE_HPP

#include <fstream>
#include <string>

#include "../enum/MinimizationTypeEnum.hpp"
#include "../enum/ProblemTypeEnum.hpp"

#include "Customer.hpp"
#include "Depot.hpp"

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