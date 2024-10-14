#ifndef FRAME_H
#define FRAME_H

#include "../problem/Problem.h"
#include "Sector.h"

class Frame : public Sector {
    public:
        Frame() {}

        Frame(Problem problem, int sectorsCount) {
            this->sectorsCount = sectorsCount;
            create(problem);
            initializeSectors();
            assignSectorToCustomers(problem);
        }

        int sectorsCount;
        int* customerSectorMap;
        Sector* sectors;

        void create(Problem problem);
        void print();
        
        void initializePositions();
        void updatePositions(Position2D position);
        
        void initializeSectors();
        void splitSectorsHorizontally(double xTotal);
        void splitSectorsVertically(double yTotal);
        void assignSectorToCustomers(Problem problem);
};

int isCustomerInSector(Sector sector, Customer customer);

#endif