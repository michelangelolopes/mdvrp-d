#include "../../include/utils/ArrayUtils.hpp"

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