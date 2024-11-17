#ifndef STODOLA_INSPIRED_ANT_SYSTEM_HPP
#define STODOLA_INSPIRED_ANT_SYSTEM_HPP

#include "../clustering/Cluster.hpp"
#include "../clustering/Frame.hpp"
#include "../problem/ProblemInstance.hpp"
#include "../solution/Solution.hpp"

#include "Pheromone.hpp"
#include "SimulatedAnnealing.hpp"

class StodolaInspiredAntSystem : public SimulatedAnnealing {
    public:
        StodolaInspiredAntSystem(
            const ProblemInstance& problemInstance, 
            int antsCount, 
            double pheromoneReinforcementCoef, 
            double temperature,
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
        SimulatedAnnealing(temperature, temperatureCoolingCoef),
        problemInstance(problemInstance),
        bestSolution(problemInstance),
        frame(problemInstance, sectorsCount),
        pheromone(problemInstance, pheromoneReinforcementCoef, pheromoneEvaporationCoefMin, pheromoneEvaporationCoefMax),
        antsCount(antsCount),
        localOptimizationFrequency(localOptimizationFrequency),
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
        double distanceProbabilityCoef;
        double pheromoneProbabilityCoef;
        double (*weightedValue)(double, double);

        int antsCount;
        int maxExchangeSuccessiveVertices;

        int maxIterations;
        int maxIterationsWithoutImprovement;
        double maxOptimizationTime;
        double minInformationEntropyCoef;

        Cluster* verticesClusters = nullptr;
        Frame frame;
        Pheromone pheromone;
        ProblemInstance problemInstance;
        Solution bestSolution;

        void finalize();
        void print();
        void run();
        void runWithDrone();
    
    private:
        void create(int primarySubClustersMaxCount, int subClusterMaxSize);
        void createClusters(int primarySubClustersMaxCount, int subClusterMaxSize);

        bool useWorseSolution(const Solution& generationBestSolution);
        int hasAchievedTerminationCondition(int iterationsCount, int iterationsWithoutImprovementCount, double currentOptimizationTime, double informationEntropyCoef);

        void buildAntRoutes(Solution& antSolution, int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage);

        int selectDepot(int* visitedCustomersIndexes, double* selectionProbability, Route* routes);
        int updateDepotSelectionProbability(int* visitedCustomersIndexes, double* selectionProbability, Route* routes);

        int selectSubCluster(int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage, int depotIndex, int vertexIndex, double*** pheromoneMatrix);
        int updatePrimarySubClusterSelectionProbability(
            int* visitedCustomersIndexes, 
            double* primarySubClusterSelectionProbability,
            double* heuristicInformationAverage, 
            double* pheromoneConcentrationAverage,
            int depotIndex,
            int vertexIndex,
            double*** pheromoneMatrix
        );
        void calculatePrimarySubClusterSelectionProbability( 
            double* primarySubClusterSelectionProbability,
            double* heuristicInformationAverage, 
            double* pheromoneConcentrationAverage,
            Cluster* cluster
        );

        int selectSubClusterNonPrimary(int* visitedCustomersIndexes, int vertexIndex);

        int selectCustomer(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int vertexIndex, int subClusterIndex);
        void updateCustomerSelectionProbability(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int vertexIndex, const SubCluster& subCluster);

        double calculateInformationEntropy(int** edgesOccurrenceCount, int generationEdgesCount);
        int updateGenerationEdgesOccurrenceCount(const Solution& solution, int** edgesOccurrenceCount);

        void localOptimization(Solution& generationBestSolution);

        void exchangeMembersInSolution(Solution& exchangeSolution);
        void exchangeMembersInRoutes(
            Solution& exchangeSolution, 
            Route& route,
            Route& exchangeRoute,
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

        void moveMembersInSolution(Solution& moveSolution);
        void moveMembersInRoutes(
            Solution& moveSolution, 
            Route& route,
            Route& destRoute,
            int successiveVerticesCount
        );
        int moveMembersInSubRoutes(
            Solution& moveSolution, 
            SubRoute& subRoute,
            SubRoute& destSubRoute,
            int successiveVerticesCount
        );

        void buildAntRoutesWithDrone(Solution& antSolution, int* visitedCustomersIndexes, double* selectionProbability, double* heuristicInformationAverage, double* pheromoneConcentrationAverage);
        int selectDroneCustomer(int* visitedCustomersIndexes, double* selectionProbability, int depotIndex, int droneSubClusterIndex, int launchVertexIndex, int recoveryVertexIndex, const Route& route);
        void updateDroneCustomerSelectionProbability(bool* candidateMembersIndex, double* customerSelectionProbability, int depotIndex, int vertexIndex, const SubCluster& subCluster);
        bool canDroneVisitCustomer(const Route& route, const Sortie& sortie, const Customer& customer, const Truck& truck, const Drone& drone);
        int updateGenerationDroneEdgesOccurrenceCount(const Solution& solution, int** edgesOccurrenceCount);
        double calculateInformationEntropyWithDrone(int** edgesOccurrenceCount, int** droneEdgesOccurrenceCount, int generationEdgesCount, int generationDroneEdgesCount);
};

#endif