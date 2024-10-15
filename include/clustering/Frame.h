#ifndef FRAME_H
#define FRAME_H

#include "../problem/ProblemInstance.h"
#include "Sector.h"

class Frame : public Sector {
    public:
        Frame(int sectorsCount) {
            this->sectorsCount = sectorsCount;
        }

        Frame(ProblemInstance problemInstance, int sectorsCount) : 
        Frame(sectorsCount) 
        {
            create(problemInstance);
        }

        int customerCount;
        int sectorsCount;
        int* customerSectorMap = nullptr;
        Sector* sectors = nullptr;

        void create(ProblemInstance problemInstance);
        void finalize();
        void print();

        void initializePositions();
        void updatePositions(Position2D position);
        
        void initializeSectors();
        void splitSectorsHorizontally(double xTotal);
        void splitSectorsVertically(double yTotal);
        void assignSectorToCustomers(ProblemInstance problemInstance);
};

int isCustomerInSector(Sector sector, Customer customer);

#endif