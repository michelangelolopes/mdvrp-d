#include <iostream>
#include <string>

#include "include/clustering/Frame.h"
#include "include/problem/Problem.h"

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

    Problem problem(datasetFilePath);
    Frame frame(problem, sectorsCount);

    problem.finalize();
    frame.finalize();

    return 0;
}