#include "../../include/clustering/Frame.hpp"

#include <cmath>
#include <iostream>
#include "../../include/utils/ArrayUtils.hpp"

void Frame::create(ProblemInstance problemInstance) {

    this->customersCount = problemInstance.customersCount;

    initializePositions();

    for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
        updatePositions(problemInstance.customers[customerIndex].position);
    }

    initializeSectors();
    assignSectorToCustomers(problemInstance);
}

void Frame::finalize() {

    if(this->sectors != nullptr) {
        free(this->sectors);
    }

    if(this->customerSectorMap != nullptr) {
        free(this->customerSectorMap);
    }
}

void Frame::print() {

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
        for(int customerIndex = 0; customerIndex < this->customersCount; customerIndex++) {
            if(customerSectorMap[customerIndex] == sectorIndex) {
                std::cout << customerIndex << "\t";
            }
        }
        std::cout << '\n';
    }

    std::cout << "--------------------------------------------------\n";
}

void Frame::initializePositions() {
    max.x = 0;
    min.x = 0;
    max.y = 0;
    min.y = 0;
}

void Frame::updatePositions(Position2D position) {
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

void Frame::initializeSectors() {
    
    double xTotal = abs(min.x) + abs(max.x);
    double yTotal = abs(min.y) + abs(max.y);

    sectors = (Sector*) calloc(sectorsCount, sizeof(Sector));

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
        
        sectorEnd = sectorStart + (sectorTotalSize - 1);

        sectors[sectorIndex] = Sector(Position2D(sectorStart, min.y), Position2D(sectorEnd, max.y));

        sectorStart = sectorEnd + 1;
    }
}

void Frame::splitSectorsVertically(double yTotal) {
    int sectorTotalSize = std::ceil(yTotal / sectorsCount);

    double sectorStart = min.y;
    double sectorEnd;

    for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
        
        sectorEnd = sectorStart + (sectorTotalSize - 1);

        sectors[sectorIndex] = Sector(Position2D(min.x, sectorStart), Position2D(max.x, sectorEnd));

        sectorStart = sectorEnd + 1;
    }
}

void Frame::assignSectorToCustomers(ProblemInstance problemInstance) {

    customerSectorMap = (int*) calloc(problemInstance.customersCount, sizeof(int));

    for(int customerIndex = 0; customerIndex < problemInstance.customersCount; customerIndex++) {
        for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
            if( isPositionInSector(sectors[sectorIndex], problemInstance.customers[customerIndex].position) ) {
                customerSectorMap[customerIndex] = sectorIndex;
                break;
            }
        }
    }
}

int isPositionInSector(Sector sector, Position2D position) {
    return (position.x >= sector.min.x) && 
        (position.x <= sector.max.x) && 
        (position.y >= sector.min.y) && 
        (position.y <= sector.max.y);
}