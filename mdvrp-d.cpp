// #include <stdio.h>

#include <string>
#include <iostream>


#include "class/array.h"
#include "class/file.h"
// #include "class/vehicle.h"
#include "class/truck.h"
#include "class/uav.h"
// #include "class/customer.h"
#include "class/customer-cluster.h"
#include "class/problem.h"

using namespace std;

int main() {

    ProblemParameters problem;
    char const* filename = "../Dataset/2_MDVRP-D/MDVRP-D-Benchmarks/p01-D.mdvrp-d";
    loadFile(filename, problem);
    printProblem(problem);

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
//oi eu sou a dani blay lopes s2