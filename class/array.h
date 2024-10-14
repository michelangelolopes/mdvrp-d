#ifndef ARRAY_H
#define ARRAY_H

#include <cstdlib>

void* initialize(int count, int sizeObject) {
    void* array = (void*) calloc(count, sizeObject);

    return array;
}

void** initialize(int count, int sizePointer, int sizeObject) {
    void** matrix = (void**) calloc(count, sizePointer);
    for(int i = 0; i < count; i++) {
        matrix[i] = initialize(count, sizeObject);
    }
    return matrix;
}

void freeMatrix(void** matrix, int count) {
    for(int i = 0; i < count; i++) {
        free(matrix[i]);
    }

    free(matrix);
}

#endif