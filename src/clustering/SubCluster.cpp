#include "../../include/clustering/SubCluster.hpp"

#include <iostream>

void SubCluster::initialize() {
    elements = (int*) malloc(size * sizeof(int));
}

void SubCluster::finalize() {
    free(elements);
}

void SubCluster::print() const {

    for(int index = 0; index < size; index++) {
        std::cout << elements[index];

        if(index < size - 1) {
            std::cout << " ";
        }
    }
}

void SubCluster::print(int* visitedCustomersIndexes) const {

    for(int index = 0; index < size; index++) {
        
        int customerIndex = elements[index];
        if(visitedCustomersIndexes[customerIndex] != 1) {
            std::cout << customerIndex;

            if(index < size - 1) {
                std::cout << " ";
            }
        }
    }
}