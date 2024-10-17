#include <iostream>
#include <string>

#include "../include/metaheuristics/StodolaInspiredAntSystem.h"
#include "../include/problem/ProblemInstance.h"

using namespace std;

int main(int argc, char** argv) {

    string datasetFilePath = "../Dataset/2_MDVRP-D/MDVRP-D-Benchmarks/";
    if(argc >= 2) {
        datasetFilePath += argv[1];
        std::cout << datasetFilePath << '\n';
    } else {
        datasetFilePath += "p01-D.mdvrp-d";
    }

    ProblemInstance problemInstance(datasetFilePath);

    int antsCount = 5; //192;
    double pheromoneUpdateCoef = 3; 
    double temperatureUpdateCoef = 0.1;
    double temperatureCoolingCoef = 1;
    int sectorsCount = 5; //16;
    int subClusterSize = 5;//24;
    int localOptimizationFrequency = 10;
    int primaryClustersCount = 4;
    double pheromoneEvaporationCoefMin = 0.001;
    double pheromoneEvaporationCoefMax = 0.1;
    double pheromoneProbabilityCoef = 1;
    double distanceProbabilityCoef = 1;
    StodolaInspiredAntSystem antSystem(
            problemInstance, 
            antsCount, 
            pheromoneUpdateCoef, 
            temperatureUpdateCoef,
            temperatureCoolingCoef,
            sectorsCount, 
            primaryClustersCount,
            subClusterSize,
            localOptimizationFrequency,
            pheromoneEvaporationCoefMin,
            pheromoneEvaporationCoefMax,
            distanceProbabilityCoef,
            pheromoneProbabilityCoef
    );

    #ifdef DEBUG
        int printDistanceMatrix = 0;
        problemInstance.print(printDistanceMatrix);
        antSystem.customerClusters[0].print();
        antSystem.customerClusters[problemInstance.customerCount].print();
        antSystem.customerClusters[problemInstance.customerCount + 1].print();
        antSystem.frame.print();
    #endif

    antSystem.run();

    antSystem.finalize();

    return 0;
}