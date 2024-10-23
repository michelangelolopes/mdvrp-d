#include <chrono>
#include <iostream>
#include <string>

#include "../include/metaheuristics/StodolaInspiredAntSystem.hpp"
#include "../include/problem/ProblemInstance.hpp"

using namespace std;

int main(int argc, char** argv) {

    string mdvrpdInstance = "./dataset/mdvrpd/p01-D.mdvrp-d";
    ProblemInstance problemInstanceMDVRPD(mdvrpdInstance, ProblemType::MDVRP_D);
    
    string mdvrpInstance = "./dataset/mdvrp/p01";
    ProblemInstance problemInstanceMDVRP(mdvrpInstance, ProblemType::MDVRP);

    int antsCount = 192; //192;
    double pheromoneUpdateCoef = 3; //3
    double temperatureUpdateCoef = 0.1; //0.1
    double temperatureCoolingCoef = 1; //1
    int sectorsCount = 10; //16;
    int subClusterMaxSize = 10;//24;
    int localOptimizationFrequency = 10; //10
    int primarySubClustersMaxCount = 2; //4
    double pheromoneEvaporationCoefMin = 0.001; //0.001
    double pheromoneEvaporationCoefMax = 0.1; //0.1
    double pheromoneProbabilityCoef = 1; //1
    double distanceProbabilityCoef = 1; //1
    int maxIterations = 100000; //100000
    int maxIterationsWithoutImprovement = 4000; //4000
    double maxOptimizationTime = 120; //20000
    double minInformationEntropyCoef = 0.5; //0.1

    auto start = std::chrono::high_resolution_clock::now();

    StodolaInspiredAntSystem antSystem(
            problemInstanceMDVRPD, 
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
            pheromoneProbabilityCoef,
            maxIterations,
            maxIterationsWithoutImprovement,
            maxOptimizationTime,
            minInformationEntropyCoef
    );

    antSystem.run();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds\n";

    antSystem.finalize();

    return 0;
}