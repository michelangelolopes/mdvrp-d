#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

#include <cstdlib>
#include <iostream>

void** callocMatrix(int count, int pointerSize, int objectSize);
void** callocMatrix(int pointerCount, int objectCount, int pointerSize, int objectSize);

template <typename T> void printIndexesArray(T* array, int count, T value) {
    
    for(int i = 0; i < count; i++) {
        
        if(array[i] == value) {
            std::cout << i << " ";
        }
    }

    std::cout << "\n";
}

template <typename T> T sumArray(T* array, int count) {
    
    T sum = 0;
    for(int i = 0; i < count; i++) {
        sum += array[i];
    }

    // return std::accumulate(array, array + count, 0);
    return sum;
}

template <typename T> T sumMatrix(T** matrix, int count) {
    
    T sum = 0;
    for(int i = 0; i < count; i++) {
        sum += sumArray(matrix[i], count);
    }

    // return std::accumulate(array, array + count, 0);
    return sum;
}

template <typename T> void fillArray(T* array, int count, T value) {

    // std::fill(array, array + count, value);
    for(int i = 0; i < count; i++) {
        array[i] = value;
    }
}

template <typename T> void fillMatrix(T** matrix, int count, T value) {

    for(int i = 0; i < count; i++) {
        fillArray(matrix[i], count, value);
    }
}

template <typename T> void freeMatrix(T** matrix, int pointerCount) {
    for(int i = 0; i < pointerCount; i++) {
        free(matrix[i]);
    }

    free(matrix);
}

#endif