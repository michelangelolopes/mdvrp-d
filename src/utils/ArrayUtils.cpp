#include "../../include/utils/ArrayUtils.h"

#include <cstdlib>

void fillArray(double* array, int count, int value) {

    for(int i = 0; i < count; i++) {
        array[i] = value;
    }
}

void fillMatrix(int** matrix, int count, int value) {

    for(int i = 0; i < count; i++) {
        for(int j = 0; j < count; j++) {
            matrix[i][j] = value;
        }
    }
}

void** callocMatrix(int count, int pointerSize, int objectSize) {

    void** matrix = (void**) malloc(count * pointerSize);
    for(int i = 0; i < count; i++) {
        matrix[i] = calloc(count, objectSize);
    }
    return matrix;
}

void** callocMatrix(int pointerCount, int objectCount, int pointerSize, int objectSize) {

    void** matrix = (void**) malloc(pointerCount * pointerSize);
    for(int i = 0; i < pointerCount; i++) {
        matrix[i] = calloc(objectCount, objectSize);
    }
    return matrix;
}

void freeMatrix(void** matrix, int pointerCount) {
    for(int i = 0; i < pointerCount; i++) {
        free(matrix[i]);
    }

    free(matrix);
}