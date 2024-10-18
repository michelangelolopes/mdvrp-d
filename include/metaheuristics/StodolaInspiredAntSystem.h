#ifndef STODOLA_INSPIRED_ANT_SYSTEM_H
#define STODOLA_INSPIRED_ANT_SYSTEM_H

#include "../clustering/Cluster.h"
#include "../clustering/Frame.h"
#include "AntSystem.h"
#include "SimulatedAnnealing.h"

class StodolaInspiredAntSystem : public AntSystem, public SimulatedAnnealing {
    public:
        StodolaInspiredAntSystem(
            ProblemInstance problemInstance, 
            int antsCount, 
            double pheromoneUpdateCoef, 
            double temperatureUpdateCoef,
            double temperatureCoolingCoef,
            int sectorsCount, 
            int primaryClustersCount,
            int subClusterSize,
            int localOptimizationFrequency,
            double pheromoneEvaporationCoefMin,
            double pheromoneEvaporationCoefMax,
            double distanceProbabilityCoef,
            double pheromoneProbabilityCoef
        ) : 
        AntSystem(problemInstance, antsCount, pheromoneUpdateCoef), 
        SimulatedAnnealing(temperatureUpdateCoef, temperatureCoolingCoef),
        frame(problemInstance, sectorsCount)
        {
            this->primaryClustersCount = primaryClustersCount;
            this->localOptimizationFrequency = localOptimizationFrequency;
            this->pheromoneEvaporationCoefMin = pheromoneEvaporationCoefMin;
            this->pheromoneEvaporationCoefMax = pheromoneEvaporationCoefMax;
            this->distanceProbabilityCoef = distanceProbabilityCoef;
            this->pheromoneProbabilityCoef = pheromoneProbabilityCoef;
            create(subClusterSize);
        }

        int localOptimizationFrequency;
        int primaryClustersCount;
        double pheromoneEvaporationCoefMin;
        double pheromoneEvaporationCoefMax;

        double distanceProbabilityCoef;
        double pheromoneProbabilityCoef;

        Cluster* customerClusters = nullptr;
        Frame frame;

        void create(int subClusterSize);
        void finalize();
        void print();

        void initializeCustomerClusters(int subClusterSize);

        void initializePheromoneMatrix();
        void updatePheromoneMatrix(Solution* consideredSolution);
        void updatePheromoneMatrixWithProbability(Solution* generationBestSolution);

        void updateEvaporationCoef(int** populationEdgesOcurrenceSum, int populationEdgesSum);
        void evaporatePheromoneMatrix();

        void run();
        Solution buildAntSolution();

        int selectDepot(int vertexIndex, int* visitedCustomerIndexes);
        int selectCluster(int vertexIndex, int* visitedCustomersIndexes, int depotIndex);
        int selectClusterNonPrimary(int vertexIndex, int* visitedCustomersIndexes);
        int selectCustomer(int vertexIndex, int* visitedCustomersIndexes, int depotIndex, int clusterIndex);

        double calculateInformationEntropy(int** populationEdgesOcurrenceSum, int populationEdgesSum);
};

void normalizeValues(int candidatesCount, double* selectionProbability);
int rouletteWheelSelection(int candidatesCount, double* selectionProbability);

#endif