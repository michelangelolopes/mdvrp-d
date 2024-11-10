#include "../../include/solution/DroneRoute.hpp"

void DroneRoute::init(int maxSize) {

    sorties = (Sortie*) malloc(maxSize * sizeof(Sortie));

    initializeValues();
}

void DroneRoute::initializeValues() {

    size = 0;
    distanceTraveled = -1;
    timeSpent = -1;
}

void DroneRoute::insert(const Sortie& sortie) {

    sorties[size++] = sortie;
}

void DroneRoute::reset() {

    initializeValues();
}

void DroneRoute::finalize() {

    if(sorties != nullptr) {
        free(sorties);
    }
}