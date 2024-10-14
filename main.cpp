// #include <stdio.h>

#include <string>
#include <iostream>


// #include "include/utils/ArrayUtils.h"
// #include "class/truck.h"
// #include "class/uav.h"
// #include "class/customer.h"
// #include "util/customer-cluster.h"
#include "include/models/Problem.h"

using namespace std;

int main(int argc, char** argv) {

    string datasetFilePath = "../Dataset/2_MDVRP-D/MDVRP-D-Benchmarks/";
    if(argc >= 2) {
        datasetFilePath += argv[1];
        std::cout << datasetFilePath << '\n';
    } else {
        datasetFilePath += "p01-D.mdvrp-d";
    }

    Problem problem(datasetFilePath);

    // problem.customerCount = 20;
    // problem.customers = (Customer*) initialize(problem.customerCount, sizeof(Customer));
    // problem.customers[0].x = 1;
    // problem.customers[0].y = 2;
    // problem.customers[1].x = 3;
    // problem.customers[1].y = 2;

    // calculateDistance(customers[0], customers[1]);

    // std::cout << problem.name << '\n';
    
    // int customerCount = 20;
    // Customer* customers = (Customer*) initialize(customerCount, sizeof(Customer));
    // float** pheromoneMatrix = (float**) initialize(customerCount, sizeof(float*), sizeof(float));

    // customers[0] = Customer(1, 2);
    // customers[1] = Customer(3, 2);

    // pheromoneMatrix[0][0] = 5;
    // Truck truck;

    // for(int customerIndex = 0; customerIndex < customerCount; customerIndex++) {
    //     int subClusterSize = 7;
    //     int clusterSectionCount = 5;
    //     std::cout << pheromoneMatrix[0][0] << "\n";
    //     CustomerCluster customerCluster = CustomerCluster::create(
    //         customerIndex,
    //         customers, 
    //         customerCount,
    //         subClusterSize, 
    //         clusterSectionCount
    //     );

    //     break;
    // }


    // free(customers);
    // freeMatrix((void**) pheromoneMatrix, customerCount);

    return 0;
}