#include "../../include/solution/HybridSubRoute.hpp"

void HybridSubRoute::init() {

    SubRoute::init();

    if(sorties == nullptr) {
        int maxSortiesCount = (problemInstance->customersCount / 2) + 1;
        sorties = (Sortie*) malloc(maxSortiesCount * sizeof(Sortie));
    }
}

void HybridSubRoute::resetValues() {

    SubRoute::resetValues();

    sortiesCount = 0;
}

void HybridSubRoute::finalize() {

    SubRoute::finalize();

    if(sorties != nullptr) {
        free(sorties);
    }
}

void HybridSubRoute::copy(const HybridSubRoute& subRouteToCopy) {

    SubRoute::copy(subRouteToCopy);

    sortiesCount = subRouteToCopy.sortiesCount;

    for(int sortieIndex = 0; sortieIndex < sortiesCount; sortieIndex++) {

        Sortie sortieToCopy = subRouteToCopy.sorties[sortieIndex];
        sorties[sortieIndex] = sortieToCopy;
    }
}

void HybridSubRoute::insertSortie(const Sortie& sortie) {

    sorties[sortiesCount++] = sortie;
}

Sortie& HybridSubRoute::lastSortie() const {
    return sorties[sortiesCount - 1];
}

void HybridSubRoute::printSorties() const {

    for(int index = 0; index < sortiesCount; index++) {
        sorties[index].print();
    }
}
