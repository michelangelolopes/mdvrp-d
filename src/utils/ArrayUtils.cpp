#include "../../include/utils/ArrayUtils.h"

#include <cstdlib>

void* initialize(int count, int objectSize) {

    return (void*) calloc(count, objectSize);
}

void** initialize(int count, int pointerSize, int objectSize) {

    void** matrix = (void**) calloc(count, pointerSize);
    for(int i = 0; i < count; i++) {
        matrix[i] = initialize(count, objectSize);
    }
    return matrix;
}

void** initialize(int pointerCount, int objectCount, int pointerSize, int objectSize) {

    void** matrix = (void**) calloc(pointerCount, pointerSize);
    for(int i = 0; i < pointerCount; i++) {
        matrix[i] = initialize(objectCount, objectSize);
    }
    return matrix;
}

void freeMatrix(void** matrix, int pointerCount) {
    for(int i = 0; i < pointerCount; i++) {
        free(matrix[i]);
    }

    free(matrix);
}