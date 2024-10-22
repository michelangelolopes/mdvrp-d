#ifndef FRAME_HPP
#define FRAME_HPP

#include "../problem/ProblemInstance.hpp"
#include "Sector.hpp"

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

        int customersCount;
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

int isPositionInSector(Sector sector, Position2D position);

#endif