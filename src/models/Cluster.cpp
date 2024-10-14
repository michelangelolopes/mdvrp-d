#ifndef CUSTOMER_CLUSTER_H
#define CUSTOMER_CLUSTER_H

#include <cmath>
#include <iostream>

#include "customer-sub-cluster.h"
#include "array.h"

double calculateEuclidianDistance(double x1, double y1, double x2, double y2) {

    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

double calculateDistance(Customer currentCustomer, Customer neighborCustomer) {

    return calculateEuclidianDistance(
        currentCustomer.x, 
        currentCustomer.y,
        neighborCustomer.x,
        neighborCustomer.y
    );
}

int findClosestCustomer(int currentCustomerIndex, int* freeCustomersIndexes, Customer* customers, int customerCount) {

    int closestCustomerIndex = -1;
    double closestCustomerDistance = -1;
    for(int customerIndex = 0; customerIndex < customerCount; customerIndex++) {
        if(freeCustomersIndexes[customerIndex] != -1) {
            double neighborCustomerDistance = calculateDistance(
                customers[currentCustomerIndex], 
                customers[customerIndex]
            );

            // std::cout << neighborCustomerDistance << '\n';

            if(closestCustomerDistance == -1 || closestCustomerDistance > neighborCustomerDistance) {
                closestCustomerIndex = customerIndex;
                closestCustomerDistance = neighborCustomerDistance;
            }
        }
    }

    return closestCustomerIndex;
}

class CustomerCluster {
    CustomerSubCluster* customerSubClusters;

    public:

        CustomerCluster() {}

        static CustomerCluster create(
            // Customer currentCustomer,
            int currentCustomerIndex,
            Customer* customers,
            int customerCount,            
            int subClusterSize,
            int clusterSectionCount
        ) {

            int clusterSize = std::ceil((float)customerCount / (float) subClusterSize);
            // std::cout << subClusterCount << '\n';
            int** cluster = (int**) initialize(clusterSize, sizeof(int*));
            int* freeCustomersIndexes = (int*) initialize(customerCount, sizeof(int));
            freeCustomersIndexes[currentCustomerIndex] = -1;

            // for(int sectionIndex = 0; sectionIndex < clusterSectionCount; sectionIndex++) {
            // }
            
            int freeCustomersCount = customerCount - 1;
            int clusterIndex = 0;
            int subClusterCount = 0;
            int* subCluster = (int*) initialize(subClusterSize, sizeof(int));
            while(freeCustomersCount > 0) {
                
                if(subClusterCount >= subClusterSize) {
                    subClusterCount = 0;
                    cluster[clusterIndex] = subCluster;
                    clusterIndex++;
                    subCluster = (int*) initialize(subClusterSize, sizeof(int));
                }

                int closestCustomerIndex = findClosestCustomer(
                    currentCustomerIndex, 
                    freeCustomersIndexes, 
                    customers, 
                    customerCount
                );

                subCluster[subClusterCount] = closestCustomerIndex;
                
                subClusterCount++;
                freeCustomersCount--;
            }

            // std::cout << closestCustomerIndex << '\n';


            return CustomerCluster(); 
        }
    
};

#endif