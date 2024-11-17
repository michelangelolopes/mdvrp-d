#ifndef ARRAY_UTILS_HPP
#define ARRAY_UTILS_HPP

#include <cstdlib>
#include <iostream>

void** mallocMatrix(int count, int pointerSize, int objectSize);
void** mallocMatrix(int pointerCount, int objectCount, int pointerSize, int objectSize);

template <typename T> void shiftLeftArray(T* array, int consideredObjectCount, int originIndex, int shiftSize) {

    for(int shiftIndex = originIndex + shiftSize; shiftIndex < consideredObjectCount; shiftIndex++) {
        
        int destIndex = shiftIndex - shiftSize;
        array[destIndex] = array[shiftIndex];
    }
}

template <typename T> void shiftRightArray(T* array, int consideredObjectCount, int originIndex, int shiftSize) {

    for(int shiftIndex = consideredObjectCount - 1; shiftIndex >= originIndex; shiftIndex--) {
        
        int destIndex = shiftIndex + shiftSize;
        array[destIndex] = array[shiftIndex];
    }
}

template <typename T> void printIndexesArray(T* array, int objectCount, T value) {
    
    for(int i = 0; i < objectCount; i++) {
        
        if(array[i] == value) {
            std::cout << i << " ";
        }
    }

    std::cout << "\n";
}

template <typename T> T sumArray(T* array, int objectCount) {
    
    T sum = 0;
    for(int i = 0; i < objectCount; i++) {
        sum += array[i];
    }

    return sum;
}

template <typename T> void transformArray(T* array, int objectCount, T coef, T (*operation)(T, T)) {

    for(int i = 0; i < objectCount; i++) {
        array[i] = operation(array[i], coef);
    }
}

template <typename T> void transformMatrix(T** matrix, int pointerCount, int objectCount, T coef, T (*operation)(T, T)) {

    for(int i = 0; i < pointerCount; i++) {
        transformArray(matrix[i], objectCount, coef, operation);
    }
}

template <typename T> void transformMatrix(T** matrix, int count, T coef, T (*operation)(T, T)) {

    transformMatrix(matrix, count, count, coef, operation);
}

template <typename T> void fillArray(T* array, int objectCount, T value) {

    for(int i = 0; i < objectCount; i++) {
        array[i] = value;
    }
}

template <typename T> void fillMatrix(T** matrix, int pointerCount, int objectCount, T value) {

    for(int i = 0; i < pointerCount; i++) {
        fillArray(matrix[i], objectCount, value);
    }
}

template <typename T> void fillMatrix(T** matrix, int count, T value) {

    fillMatrix(matrix, count, count, value);
}

template <typename T> void freeMatrix(T** matrix, int pointerCount) {

    for(int i = 0; i < pointerCount; i++) {
        free(matrix[i]);
    }

    free(matrix);
}

#endif