#include <chrono>
#include <iostream>
#include <string>

#include "../include/metaheuristics/StodolaInspiredAntSystem.h"
#include "../include/problem/ProblemInstance.h"

using namespace std;

int main(int argc, char** argv) {

    string datasetFilePath = "../Dataset/2_MDVRP-D/MDVRP-D-Benchmarks/p01-D.mdvrp-d";
    // if(argc >= 2) {
    //     datasetFilePath += argv[1];
    //     std::cout << datasetFilePath << '\n';
    // }


    ProblemInstance problemInstance(datasetFilePath);

    int antsCount = 192; //192;
    double pheromoneUpdateCoef = 3; 
    double temperatureUpdateCoef = 0.1;
    double temperatureCoolingCoef = 1;
    int sectorsCount = 16; //16;
    int subClusterMaxSize = 24;//24;
    int localOptimizationFrequency = 10;
    int primarySubClustersMaxCount = 4;
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
            primarySubClustersMaxCount,
            subClusterMaxSize,
            localOptimizationFrequency,
            pheromoneEvaporationCoefMin,
            pheromoneEvaporationCoefMax,
            distanceProbabilityCoef,
            pheromoneProbabilityCoef
    );

    // #ifdef DEBUG
    //     int printDistanceMatrix = 0;
    //     problemInstance.print(printDistanceMatrix);
    // antSystem.customerClusters[0].print();
    // std::cout << "\n";
    
    // antSystem.depotClusters[0].print();
    // std::cout << "\n";
    //     antSystem.frame.print();
    // #endif

    // Solution test(1, 7);
    // test.routes[0].visitedVertices[0] = 50;
    // test.routes[0].visitedVertices[1] = 45;
    // test.routes[0].visitedVertices[2] = 38;
    // test.routes[0].visitedVertices[3] = 18;
    // test.routes[0].visitedVertices[4] = 19;
    // test.routes[0].visitedVertices[5] = 5;
    // test.routes[0].visitedVertices[6] = 50;
    // test.routes[0].routeRealLength = 7;
    // test.calculateFitness(problemInstance);
    // test.print();
    // test.finalize();

    auto start = std::chrono::high_resolution_clock::now();

    antSystem.run();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds\n";

    antSystem.finalize();

    return 0;
}