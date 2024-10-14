#include "../../include/clustering/Frame.h"

#include <cmath>
#include <iostream>
#include "../../include/utils/ArrayUtils.h"

void Frame::create(Problem problem) {

    initializePositions();

    // for(int depotIndex = 0; depotIndex < problem.depotCount; depotIndex++) {
    //     updateFramePositions(frame, problem.depots[depotIndex]);
    // }

    for(int customerIndex = 0; customerIndex < problem.customerCount; customerIndex++) {
        updatePositions(problem.customers[customerIndex]);
    }

    #ifndef NDEBUG
    print();
    #endif
}

void Frame::print() {
    std::cout << "max.x: " << max.x << '\n';
    std::cout << "min.x: " << min.x << '\n';
    std::cout << "max.y: " << max.y << '\n';
    std::cout << "min.y: " << min.y << '\n';
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

    // xTotal = std::ceil(xTotal);
    sectors = (Sector*) initialize(sectorsCount, sizeof(Sector));

    if(xTotal >= yTotal) {
        splitSectorsHorizontally(xTotal);
    } 
    else {
        splitSectorsVertically(yTotal);
    }
}

void Frame::splitSectorsHorizontally(double xTotal) {
    int sectorTotalSize = std::ceil(xTotal / sectorsCount);

    std::cout << "sectorTotalSizeX: " << sectorTotalSize << '\n';

    double sectorStart = min.x;
    double sectorEnd;

    for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
        
        sectorEnd = sectorStart + (sectorTotalSize - 1);
        
        std::cout << "sectorX " << sectorIndex << '\n';
        std::cout << "startX: " << sectorStart << '\n';
        std::cout << "endX: " << sectorEnd << '\n';

        sectors[sectorIndex].min.x = sectorStart;
        sectors[sectorIndex].max.x = sectorEnd;
        sectors[sectorIndex].min.y = min.y;
        sectors[sectorIndex].max.y = max.y;

        sectorStart = sectorEnd + 1;
    }
}

void Frame::splitSectorsVertically(double yTotal) {
    int sectorTotalSize = std::ceil(yTotal / sectorsCount);

    std::cout << "sectorTotalSizeY: " << sectorTotalSize << '\n';

    double sectorStart = min.y;
    double sectorEnd;

    for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
        
        sectorEnd = sectorStart + (sectorTotalSize - 1);
        
        std::cout << "sectorY " << sectorIndex << '\n';
        std::cout << "startY: " << sectorStart << '\n';
        std::cout << "endY: " << sectorEnd << '\n';

        sectors[sectorIndex].min.y = sectorStart;
        sectors[sectorIndex].max.y = sectorEnd;
        sectors[sectorIndex].min.x = min.x;
        sectors[sectorIndex].max.x = max.x;

        sectorStart = sectorEnd + 1;
    }
}

void Frame::assignSectorToCustomers(Problem problem) {

    customerSectorMap = (int*) initialize(problem.customerCount, sizeof(int));

    for(int customerIndex = 0; customerIndex < problem.customerCount; customerIndex++) {
        for(int sectorIndex = 0; sectorIndex < sectorsCount; sectorIndex++) {
            if( isCustomerInSector(sectors[sectorIndex], problem.customers[customerIndex]) ) {
                customerSectorMap[customerIndex] = sectorIndex;
                break;
            }
        }
    }

    for(int customerIndex = 0; customerIndex < problem.customerCount; customerIndex++) {
        std::cout << "customerIndex " << customerIndex << " sectorIndex " << customerSectorMap[customerIndex] << '\n';
    }
}

int isCustomerInSector(Sector sector, Customer customer) {
    return (customer.x >= sector.min.x) && 
        (customer.x <= sector.max.x) && 
        (customer.y >= sector.min.y) && 
        (customer.y <= sector.max.y);
}