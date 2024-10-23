#include "../../include/utils/ArrayUtils.hpp"

void** mallocMatrix(int pointerCount, int objectCount, int pointerSize, int objectSize) {

    void** matrix = (void**) malloc(pointerCount * pointerSize);
    for(int i = 0; i < pointerCount; i++) {
        matrix[i] = malloc(objectCount * objectSize);
    }
    return matrix;
}