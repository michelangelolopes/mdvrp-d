#include "../../include/utils/ArrayUtils.h"

#include <cstdlib>

void* initialize(int count, int sizeObject) {

    return (void*) calloc(count, sizeObject);
}

void** initialize(int count, int sizePointer, int sizeObject) {

    void** matrix = (void**) calloc(count, sizePointer);
    for(int i = 0; i < count; i++) {
        matrix[i] = initialize(count, sizeObject);
    }
    return matrix;
}

void freeMatrix(void** matrix, int pointerCount) {
    for(int i = 0; i < pointerCount; i++) {
        free(matrix[i]);
    }

    free(matrix);
}