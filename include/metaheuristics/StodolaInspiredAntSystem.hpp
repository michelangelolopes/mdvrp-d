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

        void buildAntRoutes(Solution& antSolution, int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage);

        int selectDepot(int* visitedCustomersIndexes, double* selectionProbability, Route* routes);
        int updateDepotSelectionProbability(int* visitedCustomersIndexes, double* selectionProbability, Route* routes);

        int selectSubCluster(int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage, int depotIndex, int vertexIndex);
        int updatePrimarySubClusterSelectionProbability(
            int* visitedCustomersIndexes, 
            double* primarySubClusterSelectionProbability,
            double* heuristicInformationAverage, 
            double* pheromoneConcentrationAverage,
            int depotIndex,
            int vertexIndex
        );

        int selectSubClusterNonPrimary(int* visitedCustomersIndexes, int vertexIndex);

        int selectCustomer(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int vertexIndex, int subClusterIndex);
        double* updateCustomerSelectionProbability(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int vertexIndex, SubCluster* subCluster);

        double calculateInformationEntropy(int** populationEdgesOcurrenceSum, int populationEdgesSum);
        int updateGenerationEdgesOccurrenceCount(const Solution& solution, int** edgesOcurrenceCount);
};

void normalizeValues(double* selectionProbability, int candidatesCount);
int rouletteWheelSelection(double* selectionProbability, int candidatesCount);

#endif