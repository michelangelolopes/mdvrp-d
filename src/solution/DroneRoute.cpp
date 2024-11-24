#include "../../include/solution/DroneRoute.hpp"

void DroneRoute::init() {

    int maxSortiesCount = (problemInstance->customersCount / 2) + 1;
    sorties = (Sortie*) malloc(maxSortiesCount * sizeof(Sortie));

    initializeValues();
}

void DroneRoute::initializeValues() {

    size = 0;
}

void DroneRoute::finalize() {

    if(sorties != nullptr) {
        free(sorties);
    }
}

void DroneRoute::reset() {

    initializeValues();
}

void DroneRoute::copy(const DroneRoute& droneRouteToCopy) {
    
    size = droneRouteToCopy.size;
    depotIndex = droneRouteToCopy.depotIndex;

    for(int sortieIndex = 0; sortieIndex < size; sortieIndex++) {

        Sortie sortieToCopy = droneRouteToCopy.sorties[sortieIndex];
        sorties[sortieIndex] = sortieToCopy;
    }
}

void DroneRoute::insert(const Sortie& sortie) {

    sorties[size++] = sortie;
}

void DroneRoute::updateRecoveryVertexIndex(int recoveryVertexIndex) {
    sorties[size - 1].recoveryVertexIndex = recoveryVertexIndex;
}

void DroneRoute::print() const {

    for(int index = 0; index < size; index++) {
        sorties[index].print();
    }
}
