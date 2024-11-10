#ifndef SORTIE_HPP
#define SORTIE_HPP

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

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

        void print() const {

            std::cout << toString();
        }

        string toString() const {

            return "(" + 
                to_string(launchVertexIndex) + ", " +
                to_string(deliveryVertexIndex) + ", " +
                to_string(recoveryVertexIndex) +
                ")";
        }
};

#endif