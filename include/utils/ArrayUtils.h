#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

void fillArray(double* array, int count, int value);
void fillMatrix(int** matrix, int count, int value);
void** callocMatrix(int count, int pointerSize, int objectSize);
void** callocMatrix(int pointerCount, int objectCount, int pointerSize, int objectSize);
void freeMatrix(void** matrix, int pointerCount);

#endif