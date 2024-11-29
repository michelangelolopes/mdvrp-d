#ifndef ANT_SYSTEM_HPP
#define ANT_SYSTEM_HPP

#include "../clustering/Cluster.hpp"
#include "../clustering/Frame.hpp"
#include "../problem/ProblemInstance.hpp"
#include "../solution/Solution.hpp"

#include "InformationEntropy.hpp"
#include "LocalOptimization.hpp"
#include "Pheromone.hpp"
#include "SimulatedAnnealing.hpp"

class AntSystem : public SimulatedAnnealing {
    public:
        AntSystem(
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
            int maxSuccessiveVertices,
            int maxIterations,
            int maxIterationsWithoutImprovement,
            double maxOptimizationTime,
            double minInformationEntropyCoef
        ) :
        SimulatedAnnealing(temperature, temperatureCoolingCoef),
        problemInstance(problemInstance),
        antSolution(problemInstance),
        generationBestSolution(problemInstance),
        bestSolution(problemInstance),
        localOptimization(problemInstance, maxSuccessiveVertices),
        frame(problemInstance, sectorsCount),
        pheromone(problemInstance, pheromoneReinforcementCoef, pheromoneEvaporationCoefMin, pheromoneEvaporationCoefMax),
        informationEntropy(problemInstance, antsCount),
        antsCount(antsCount),
        localOptimizationFrequency(localOptimizationFrequency),
        distanceProbabilityCoef(distanceProbabilityCoef),
        pheromoneProbabilityCoef(pheromoneProbabilityCoef),
        maxSuccessiveVertices(maxSuccessiveVertices),
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
        int maxSuccessiveVertices;

        int maxIterations;
        int maxIterationsWithoutImprovement;
        double maxOptimizationTime;
        double minInformationEntropyCoef;

        int* visitedCustomersIndexes;
        double* selectionProbability;
        double* heuristicInformationAverage;
        double* pheromoneConcentrationAverage;

        Cluster* verticesClusters = nullptr;
        Frame frame;
        LocalOptimization localOptimization;
        InformationEntropy informationEntropy;
        Pheromone pheromone;
        ProblemInstance problemInstance;
        Solution generationBestSolution;
        Solution bestSolution;
        Solution antSolution;

        void finalize();
        void run();
        void runWithDrone();
    
    private:
        void create(int primarySubClustersMaxCount, int subClusterMaxSize);
        void createClusters(int primarySubClustersMaxCount, int subClusterMaxSize);

        void buildAntRoutes(Solution& antSolution);
        void buildAntRoutesWithDrone(Solution& antSolution);

        int selectDepot(Route* routes);
        int updateDepotSelectionProbability(Route* routes);

        int selectSubCluster(int depotIndex, int vertexIndex, double*** pheromoneMatrix);
        int updatePrimarySubClusterSelectionProbability(int depotIndex, int vertexIndex, double*** pheromoneMatrix);
        void calculatePrimarySubClusterSelectionProbability(Cluster* cluster);
        int selectSubClusterNonPrimary(int vertexIndex);

        int selectCustomer(int depotIndex, int vertexIndex, int subClusterIndex);
        void updateCustomerSelectionProbability(int depotIndex, int vertexIndex, const SubCluster& subCluster);

        int selectDroneCustomer(int depotIndex, int droneSubClusterIndex, int launchVertexIndex, int recoveryVertexIndex, const Route& route);
        void updateDroneCustomerSelectionProbability(bool* candidateMembersIndex, int depotIndex, int vertexIndex, const SubCluster& subCluster);

        bool hasAchievedTerminationCondition(int iterationsCount, int iterationsWithoutImprovementCount, double currentOptimizationTime, double informationEntropyCoef);
        bool canUseWorseSolution(const Solution& generationBestSolution);
};

#endif