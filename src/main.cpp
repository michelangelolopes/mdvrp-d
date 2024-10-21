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


    ProblemInstance problemInstance(datasetFilePath, ProblemType::MDVRP_D);

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
    int maxIterations = 100000;
    int maxIterationsWithoutImprovement = 8000; //4000
    double maxOptimizationTime = 20000; //20000
    double minInformationEntropyCoef = 0.65; //0.1

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
            maxIterations,
            maxIterationsWithoutImprovement,
            maxOptimizationTime,
            minInformationEntropyCoef
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

    // Solution test2(problemInstance.depotsCount, MinimizationType::MAX_TIME_SPENT, problemInstance.customersCount);

    // test2.routes[0].insert(43);
    // test2.routes[0].insert(41);
    // test2.routes[0].insert(39);
    // test2.routes[0].insert(18);
    // test2.routes[0].insert(40);
    // test2.routes[0].expand();
    // test2.routes[0].insert(3);
    // test2.routes[0].insert(16);
    // test2.routes[0].insert(36);
    // test2.routes[0].insert(11);
    // test2.routes[0].insert(45);
    // test2.routes[0].insert(46);
    // test2.routes[0].expand();
    // test2.routes[0].insert(17);
    // test2.routes[0].insert(12);
    // test2.routes[0].shrink();

    // test2.routes[1].insert(13);
    // test2.routes[1].insert(24);
    // test2.routes[1].insert(5);
    // test2.routes[1].insert(23);
    // test2.routes[1].expand();
    // test2.routes[1].insert(22);
    // test2.routes[1].insert(6);
    // test2.routes[1].insert(42);
    // test2.routes[1].insert(47);
    // test2.routes[1].insert(0);
    // test2.routes[1].expand();
    // test2.routes[1].insert(26);
    // test2.routes[1].shrink();

    // test2.routes[2].insert(8);
    // test2.routes[2].insert(29);
    // test2.routes[2].insert(48);
    // test2.routes[2].insert(37);
    // test2.routes[2].expand();
    // test2.routes[2].insert(4);
    // test2.routes[2].insert(10);
    // test2.routes[2].insert(14);
    // test2.routes[2].insert(9);
    // test2.routes[2].expand();
    // test2.routes[2].insert(33);
    // test2.routes[2].insert(38);
    // test2.routes[2].insert(32);
    // test2.routes[2].insert(44);
    // test2.routes[2].shrink();

    // test2.routes[3].insert(27);
    // test2.routes[3].insert(30);
    // test2.routes[3].insert(25);
    // test2.routes[3].insert(7);
    // test2.routes[3].insert(31);
    // test2.routes[3].insert(21);
    // test2.routes[3].expand();
    // test2.routes[3].insert(1);
    // test2.routes[3].insert(15);
    // test2.routes[3].insert(49);
    // test2.routes[3].insert(20);
    // test2.routes[3].insert(28);
    // test2.routes[3].expand();
    // test2.routes[3].insert(19);
    // test2.routes[3].insert(34);
    // test2.routes[3].insert(35);
    // test2.routes[3].insert(2);
    // test2.routes[3].shrink();

    // test2.updateFitness(problemInstance);
    // test2.print();

    // Route[0]: D 43 41 39 18 40 D 3 16 36 11 45 46 D 17 12 D
    // Route[1]: D 13 24 5 23 D 22 6 42 47 0 D 26 D
    // Route[2]: D 8 29 48 37 D 4 10 14 9 D 33 38 32 44 D
    // Route[3]: D 27 30 25 7 31 21 D 1 15 49 20 28 D 19 34 35 2 D

    antSystem.run();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds\n";

    antSystem.finalize();

    return 0;
}