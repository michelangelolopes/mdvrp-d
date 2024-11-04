#ifndef SORTIE_HPP
#define SORTIE_HPP

#include <cstdlib>
#include <iostream>

#include "../problem/ProblemInstance.hpp"

class Sortie {
    public:
        Sortie(int launchVertexIndex, int deliveryVertexIndex, int recoveryVertexIndex) : 
        launchVertexIndex(launchVertexIndex),
        deliveryVertexIndex(deliveryVertexIndex),
        recoveryVertexIndex(recoveryVertexIndex) 
        {}

        int launchVertexIndex;
        int deliveryVertexIndex;
        int recoveryVertexIndex;
};

#endif