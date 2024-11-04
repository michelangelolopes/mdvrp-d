#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../include/metaheuristics/StodolaInspiredAntSystem.hpp"
#include "../include/problem/ProblemInstance.hpp"

using namespace std;

void loadExampleSolution(string filename, ProblemInstance problemInstance);

int main(int argc, char** argv) {

    srand((unsigned int)time(0));

    if(argc != 9) {
        std::cout << "ERROR! The program needs more args...\n";
        exit(0);
    }

    string problemName = argv[1];
    string problemInstanceIndex = argv[2];
    string problemInstanceFilePath = "./dataset";
    ProblemType problemType;

    if(problemName.compare("mdvrp") == 0) {

        problemInstanceFilePath += "/mdvrp";
        problemInstanceFilePath += "/p" + problemInstanceIndex;
        problemType = ProblemType::MDVRP;
    } else if(problemName.compare("mdvrp-d") == 0) {

        problemInstanceFilePath += "/mdvrp-d";
        problemInstanceFilePath += "/p" + problemInstanceIndex + "-D.mdvrp-d";
        problemType = ProblemType::MDVRP_D;
    } else {
        std::cout << "ERROR! The problem name must be one of the following: mdvrp, mdvrp-d\n";
        exit(0);
    }

    ProblemInstance problemInstance(problemInstanceFilePath, problemType);

    int antsCount = atoi(argv[3]); //192;
    double pheromoneUpdateCoef = atoi(argv[4]); //3
    double temperatureUpdateCoef = 0.1; //0.1
    double temperatureCoolingCoef = 1; //1
    int sectorsCount = atoi(argv[5]);//10; //16;
    int subClusterMaxSize = atoi(argv[6]);//10;//24;
    int localOptimizationFrequency = 10; //10
    int primarySubClustersMaxCount = atoi(argv[7]);//2; //4
    double pheromoneEvaporationCoefMin = 0.001; //0.001
    double pheromoneEvaporationCoefMax = 0.1; //0.1
    double pheromoneProbabilityCoef = 1; //1
    double distanceProbabilityCoef = 1; //1
    int maxExchangeSuccessiveVertices = 2; //2
    int maxIterations = 100000; //100000
    int maxIterationsWithoutImprovement = 100000; //4000
    double maxOptimizationTime = atoi(argv[8]); //60; //20000
    double minInformationEntropyCoef = 0.1; //0.1

    std::cout << "--------------------------------------------------\n";
    std::cout << "antsCount: " << antsCount << "\n";
    std::cout << "pheromoneUpdateCoef: " << pheromoneUpdateCoef << "\n";
    std::cout << "sectorsCount: " << sectorsCount << "\n";
    std::cout << "subClusterMaxSize: " << subClusterMaxSize << "\n";
    std::cout << "primarySubClustersMaxCount: " << primarySubClustersMaxCount << "\n";
    std::cout << "maxOptimizationTime: " << maxOptimizationTime << "\n";
    std::cout << "--------------------------------------------------\n";

    auto start = std::chrono::high_resolution_clock::now();

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
            pheromoneProbabilityCoef,
            maxExchangeSuccessiveVertices,
            maxIterations,
            maxIterationsWithoutImprovement,
            maxOptimizationTime,
            minInformationEntropyCoef
    );

    antSystem.run();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    antSystem.finalize();

    return 0;
}

void loadExampleSolution(string filename, ProblemInstance problemInstance) {

    ifstream file;
    string line;

    file.open(filename);
    std::getline(file, line); //ignore fitness

    Solution example(
        problemInstance.depotsCount,
        MinimizationType::TOTAL_DISTANCE_TRAVELED,
        problemInstance.customersCount
    );

    int depotIndex;
    int vertexIndex;
    int subRouteIndex;
    int tempInt;
    double tempDouble;

    while (std::getline(file, line)) {
        
        // std::cout << "line: " << line << "\n";
        istringstream valueStream(line);

        valueStream >> depotIndex;
        depotIndex--;

        // std::cout << "depotIndex: " << depotIndex << "\n";

        Route* route = &example.routes[depotIndex];

        // std::cout << "route->size: " << route->size << "\n";

        valueStream >> subRouteIndex; //subroute index

        if(subRouteIndex > 1) {
            route->expand();
        }

        // std::cout << "subRouteIndex: " << subRouteIndex << "\n";

        valueStream >> tempDouble; //subroute fitness
        valueStream >> tempInt; //subroute load
        while(valueStream >> vertexIndex) {

            if(vertexIndex > 0) {
                vertexIndex--;
                route->insert(vertexIndex);
                route->incrementCurrentLoad(problemInstance.customers[vertexIndex].demand);
            }
        }

    }
    
    example.updateFitness(problemInstance);
    example.print();
}