#ifndef FRAME_HPP
#define FRAME_HPP

#include "../problem/ProblemInstance.hpp"
#include "Sector.hpp"

class Frame : public Sector {
    public:
        Frame(int sectorsCount) : sectorsCount(sectorsCount) {}
        Frame(const ProblemInstance& problemInstance, int sectorsCount) : 
        Frame(sectorsCount) 
        {
            create(problemInstance);
        }

        int customersCount;
        int sectorsCount;
        int* customerSectorMap = nullptr;
        Sector* sectors = nullptr;

        void finalize();
        void print();

    private:
        void create(const ProblemInstance& problemInstance);

        void initializePositions();
        void updatePositions(const Position2D& position);
        
        void initializeSectors();
        void splitSectorsHorizontally(double xTotal);
        void splitSectorsVertically(double yTotal);
        void assignSectorToCustomers(const ProblemInstance& problemInstance);
};

#endif