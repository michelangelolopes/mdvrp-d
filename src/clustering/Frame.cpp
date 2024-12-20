#include "../../include/clustering/Frame.hpp"

#include <cmath>
#include <iostream>
#include "../../include/utils/ArrayUtils.hpp"

void Frame::create() {

    initializePositions();
    initializeSectors();
    assignSectorToCustomers();
}

void Frame::finalize() {

    if(sectors != nullptr) {
        free(sectors);
    }

    if(customerSectorMap != nullptr) {
        free(customerSectorMap);
    }
}

void Frame::print() const {

    std::cout << "\n--------------------------------------------------\n";

    std::cout << "Frame: ";
    std::cout << "(" << min.x << ", " << min.y << ") - "; 
    std::cout << "(" << max.x << ", " << max.y << ")\n";

    std::cout << "-------------------------\n";
    for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
        std::cout << "Sector[" << sectorIndex << "]: ";
        sectors[sectorIndex].print();
        std::cout << '\n';
    }

    std::cout << "-------------------------\n";
    for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
        std::cout << "Sector[" << sectorIndex << "]: ";
        for(int customerIndex = 0; customerIndex < problemInstance->customersCount; customerIndex++) {
            if(customerSectorMap[customerIndex] == sectorIndex) {
                std::cout << customerIndex << "\t";
            }
        }
        std::cout << '\n';
    }

    std::cout << "--------------------------------------------------\n";
}

void Frame::initializePositions() {

    for(int customerIndex = 0; customerIndex < problemInstance->customersCount; customerIndex++) {
        
        const Position2D& position = problemInstance->customers[customerIndex].position;
        
        if(position.x > max.x) {
            max.x = position.x;
        }

        if(position.x < min.x) {
            min.x = position.x;
        }

        if(position.y > max.y) {
            max.y = position.y;
        }

        if(position.y < min.y) {
            min.y = position.y;
        }
    }
}

void Frame::initializeSectors() {
    
    double xTotal = abs(min.x) + abs(max.x);
    double yTotal = abs(min.y) + abs(max.y);

    sectors = (Sector*) malloc(sectorsCount * sizeof(Sector));

    if(xTotal >= yTotal) {
        splitSectorsHorizontally(xTotal);
    } 
    else {
        splitSectorsVertically(yTotal);
    }
}

void Frame::splitSectorsHorizontally(double xTotal) {
    int sectorTotalSize = std::ceil(xTotal / sectorsCount);

    double sectorStart = min.x;
    double sectorEnd;

    for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
        
        sectorEnd = sectorStart + sectorTotalSize;

        sectors[sectorIndex] = Sector(Position2D(sectorStart, min.y), Position2D(sectorEnd, max.y));

        sectorStart = sectorEnd + 1;
    }
}

void Frame::splitSectorsVertically(double yTotal) {
    int sectorTotalSize = std::ceil(yTotal / sectorsCount);

    double sectorStart = min.y;
    double sectorEnd;

    for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
        
        sectorEnd = sectorStart + sectorTotalSize;

        sectors[sectorIndex] = Sector(Position2D(min.x, sectorStart), Position2D(max.x, sectorEnd));

        sectorStart = sectorEnd + 1;
    }
}

void Frame::assignSectorToCustomers() {

    customerSectorMap = (int*) malloc(problemInstance->customersCount * sizeof(int));

    for(int customerIndex = 0; customerIndex < problemInstance->customersCount; customerIndex++) {
        for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
            if( sectors[sectorIndex].contains(problemInstance->customers[customerIndex].position) ) {
                customerSectorMap[customerIndex] = sectorIndex;
                break;
            }
        }
    }
}