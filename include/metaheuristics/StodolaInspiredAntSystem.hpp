#ifndef STODOLA_INSPIRED_ANT_SYSTEM_HPP
#define STODOLA_INSPIRED_ANT_SYSTEM_HPP

#include "../clustering/Cluster.hpp"
#include "../clustering/Frame.hpp"
#include "AntSystem.hpp"
#include "SimulatedAnnealing.hpp"

class StodolaInspiredAntSystem : public AntSystem, public SimulatedAnnealing {
    public:
        StodolaInspiredAntSystem(
            const ProblemInstance& problemInstance, 
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
            int maxExchangeSuccessiveVertices,
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
        maxExchangeSuccessiveVertices(maxExchangeSuccessiveVertices),
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
        double pheromoneEvaporationCoefMin;
        double pheromoneEvaporationCoefMax;

        double distanceProbabilityCoef;
        double pheromoneProbabilityCoef;

        int maxExchangeSuccessiveVertices;

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
        void updatePheromoneMatrix(const Solution& consideredSolution, double updateValue, int isSumOperation);

        void reinforcePheromoneMatrixWithProbability(const Solution& generationBestSolution);
        void reinforcePheromoneMatrix(const Solution& consideredSolution);

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
        void calculatePrimarySubClusterSelectionProbability( 
            double* primarySubClusterSelectionProbability,
            double* heuristicInformationAverage, 
            double* pheromoneConcentrationAverage,
            Cluster* cluster
        );
        void calculatePrimarySubClusterSelectionProbabilityWithCoefOne( 
            double* primarySubClusterSelectionProbability,
            double* heuristicInformationAverage, 
            double* pheromoneConcentrationAverage,
            Cluster* cluster
        );

        int selectSubClusterNonPrimary(int* visitedCustomersIndexes, int vertexIndex);

        int selectCustomer(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int vertexIndex, int subClusterIndex);
        void updateCustomerSelectionProbability(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int vertexIndex, const SubCluster& subCluster);
        void updateCustomerSelectionProbabilityWithCoefOne(int* visitedCustomersIndexes, double* customerSelectionProbability, int depotIndex, int vertexIndex, const SubCluster& subCluster);

        double calculateInformationEntropy(int** edgesOcurrenceCount, int generationEdgesCount);
        int updateGenerationEdgesOccurrenceCount(const Solution& solution, int** edgesOcurrenceCount);

        void localOptimization(Solution& generationBestSolution);

        void exchangeMembersInSameRoute(
            Solution& exchangeSolution, 
            Route& route,
            int successiveVerticesCount
        );
        void exchangeMembersInSameSubRoute(
            Solution& exchangeSolution, 
            SubRoute& subRoute,
            int successiveVerticesCount
        );
        void exchangeMembersInDifferentSubRoutes(
            Solution& exchangeSolution, 
            SubRoute& subRoute,
            SubRoute& exchangeSubRoute,
            int successiveVerticesCount
        );
};

void normalizeValues(double* selectionProbability, int candidatesCount);
int rouletteWheelSelection(double* selectionProbability, int candidatesCount);

#endif