#ifndef STODOLA_INSPIRED_ANT_SYSTEM_HPP
#define STODOLA_INSPIRED_ANT_SYSTEM_HPP

#include "../clustering/Cluster.hpp"
#include "../clustering/Frame.hpp"
#include "AntSystem.hpp"
#include "SimulatedAnnealing.hpp"

class StodolaInspiredAntSystem : public AntSystem, public SimulatedAnnealing {
    public:
        StodolaInspiredAntSystem(
            ProblemInstance problemInstance, 
            int antsCount, 
            double pheromoneUpdateCoef, 
            double temperatureUpdateCoef,
            double temperatureCoolingCoef,
            int sectorsCount, 
            int primarySubClustersMaxCount,
            int subClusterMaxSize,
            int localOptimizationFrequency,
            double pheromoneEvaporationCoefMin,
            double pheromoneEvaporationCoefMax,
            double distanceProbabilityCoef,
            double pheromoneProbabilityCoef,
            int maxIterations,
            int maxIterationsWithoutImprovement,
            double maxOptimizationTime,
            double minInformationEntropyCoef
        ) : 
        AntSystem(problemInstance, antsCount, pheromoneUpdateCoef), 
        SimulatedAnnealing(temperatureUpdateCoef, temperatureCoolingCoef),
        frame(problemInstance, sectorsCount),
        localOptimizationFrequency(localOptimizationFrequency),
        pheromoneEvaporationCoefMin(pheromoneEvaporationCoefMin),
        pheromoneEvaporationCoefMax(pheromoneEvaporationCoefMax),
        distanceProbabilityCoef(distanceProbabilityCoef),
        pheromoneProbabilityCoef(pheromoneProbabilityCoef),
        maxIterations(maxIterations),
        maxIterationsWithoutImprovement(maxIterationsWithoutImprovement),
        maxOptimizationTime(maxOptimizationTime),
        minInformationEntropyCoef(minInformationEntropyCoef)
        {
            if(sectorsCount > subClusterMaxSize) {
                std::cout << "Sectors will not be used when clustering\n";
            }

            create(primarySubClustersMaxCount, subClusterMaxSize);
        }

        int localOptimizationFrequency;
        // int primarySubClustersCount;
        double pheromoneEvaporationCoefMin;
        double pheromoneEvaporationCoefMax;

        double distanceProbabilityCoef;
        double pheromoneProbabilityCoef;

        int maxIterations;
        int maxIterationsWithoutImprovement;
        double maxOptimizationTime;
        double minInformationEntropyCoef;

        Cluster* verticesClusters = nullptr;
        Frame frame;

        void finalize();
        void print();
        void run();
    
    private:
        void create(int primarySubClustersMaxCount, int subClusterMaxSize);
        void createClusters(int primarySubClustersMaxCount, int subClusterMaxSize);

        void initializePheromoneMatrices();
        void updatePheromoneMatrix(Solution* consideredSolution, double updateValue, int isSumOperation);

        void reinforcePheromoneMatrixWithProbability(Solution* generationBestSolution);
        void reinforcePheromoneMatrix(Solution* consideredSolution);

        void evaporatePheromoneMatrix();

        void updateEvaporationCoef(double informationEntropy, double informationEntropyMin, double informationEntropyMax);

        int hasAchievedTerminationCondition(int iterationsCount, int iterationsWithoutImprovementCount, double currentOptimizationTime, double informationEntropyCoef);
        int isInformationEntropySufficient(double informationEntropyCoef);

        void buildAntRoutes(Solution& antSolution, int* visitedCustomersIndexes);

        int selectDepot(int* visitedCustomersIndexes, Route* routes);
        int updateDepotSelectionProbabilityFromCustomerSource(int* visitedCustomersIndexes, double* depotSelectionProbability, int depotIndex, int customerIndex);

        int selectSubCluster(int* visitedCustomersIndexes, int vertexIndex, int depotIndex);
        double* getPrimarySubClusterSelectionProbability(double* heuristicInformationAverage, double* pheromoneConcentrationAverage, int primarySubClustersCount);
        double* getPrimarySubClusterSelectionProbabilityFromCustomerSource(
            int* visitedCustomersIndexes, 
            Cluster* cluster,
            int depotIndex,
            int customerIndex
        );

        int selectSubClusterNonPrimaryFromCustomerSource(int* visitedCustomersIndexes, int customerIndex);

        int selectCustomer(int* visitedCustomersIndexes, int vertexIndex, int depotIndex, int clusterIndex);
        double* getCustomerSelectionProbabilityFromCustomerSource(int* visitedCustomersIndexes, SubCluster* subCluster, int depotIndex, int customerIndex);

        double calculateInformationEntropy(int** populationEdgesOcurrenceSum, int populationEdgesSum);
        int updateGenerationEdgesOccurrenceCount(const Solution& solution, int** edgesOcurrenceCount);
};

void normalizeValues(double* selectionProbability, int candidatesCount);
int rouletteWheelSelection(double* selectionProbability, int candidatesCount);

#endif