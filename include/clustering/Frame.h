#ifndef FRAME_H
#define FRAME_H

#include "../problem/ProblemInstance.h"
#include "Sector.h"

class Frame : public Sector {
    public:
        Frame() {}

        Frame(ProblemInstance problemInstance, int sectorsCount) {
            this->sectorsCount = sectorsCount;
            create(problemInstance);
            initializeSectors();
            assignSectorToCustomers(problemInstance);
        }

        int sectorsCount;
        int* customerSectorMap = nullptr;
        Sector* sectors = nullptr;

        void create(ProblemInstance problemInstance);
        void print();
        void finalize();
        
        void initializePositions();
        void updatePositions(Position2D position);
        
        void initializeSectors();
        void splitSectorsHorizontally(double xTotal);
        void splitSectorsVertically(double yTotal);
        void assignSectorToCustomers(ProblemInstance problemInstance);
};

int isCustomerInSector(Sector sector, Customer customer);

#endif