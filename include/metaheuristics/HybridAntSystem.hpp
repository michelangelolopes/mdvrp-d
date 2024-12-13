#ifndef HYBRID_ANT_SYSTEM_HPP
#define HYBRID_ANT_SYSTEM_HPP

#include "HybridInformationEntropy.hpp"
#include "HybridPheromone.hpp"
#include "AntSystem.hpp"

class HybridAntSystem : public AntSystem {
    public:
        HybridAntSystem(
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
            AntSystem(problemInstance, 
            antsCount, 
            pheromoneReinforcementCoef, 
            temperature,
            temperatureCoolingCoef,
            sectorsCount, 
            primarySubClustersMaxCount,
            subClusterMaxSize,
            localOptimizationFrequency,
            pheromoneEvaporationCoefMin,
            pheromoneEvaporationCoefMax,
            distanceProbabilityCoef,
            pheromoneProbabilityCoef,
            maxSuccessiveVertices,
            maxIterations,
            maxIterationsWithoutImprovement,
            maxOptimizationTime,
            minInformationEntropyCoef),
            pheromone(problemInstance, pheromoneReinforcementCoef, pheromoneEvaporationCoefMin, pheromoneEvaporationCoefMax),
            informationEntropy(problemInstance, antsCount)
        {}

        HybridInformationEntropy informationEntropy;
        HybridPheromone pheromone;

        void run();
    
    protected:
        void buildAntRoutes(Solution& antSolution);
        
        int selectDroneCustomer(int depotIndex, int droneSubClusterIndex, int launchVertexIndex, int recoveryVertexIndex, const SubRoute& subRoute);
        void updateDroneCustomerSelectionProbability(bool* candidateMembersIndex, int depotIndex, int vertexIndex, const SubCluster& subCluster);
};

#endif