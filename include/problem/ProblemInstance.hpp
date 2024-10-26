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