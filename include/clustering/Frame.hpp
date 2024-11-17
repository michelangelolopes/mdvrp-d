#ifndef FRAME_HPP
#define FRAME_HPP

#include "../problem/ProblemInstance.hpp"
#include "Sector.hpp"

class Frame : public Sector {
    public:
        Frame(const ProblemInstance& problemInstance, int sectorsCount) :
        sectorsCount(sectorsCount), problemInstance(&problemInstance)
        {
            create();
        }

        int sectorsCount;
        int* customerSectorMap = nullptr;
        Sector* sectors = nullptr;

        void finalize();
        void print() const;

    private:
        const ProblemInstance* problemInstance;

        void create();
        void initializePositions();
        
        void initializeSectors();
        void splitSectorsHorizontally(double xTotal);
        void splitSectorsVertically(double yTotal);
        void assignSectorToCustomers();
};

#endif