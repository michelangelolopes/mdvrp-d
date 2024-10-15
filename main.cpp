#include <iostream>
#include <string>

#include "include/metaheuristics/StodolaInspiredAntSystem.h"
#include "include/problem/ProblemInstance.h"

using namespace std;

int main(int argc, char** argv) {

    string datasetFilePath = "../Dataset/2_MDVRP-D/MDVRP-D-Benchmarks/";
    if(argc >= 2) {
        datasetFilePath += argv[1];
        std::cout << datasetFilePath << '\n';
    } else {
        datasetFilePath += "p01-D.mdvrp-d";
    }

    int sectorsCount = 7;
    int subClusterSize = 7;

    ProblemInstance problemInstance(datasetFilePath);
    StodolaInspiredAntSystem antSystem(problemInstance, sectorsCount, subClusterSize);

    problemInstance.finalize();
    antSystem.finalize();

    return 0;
}