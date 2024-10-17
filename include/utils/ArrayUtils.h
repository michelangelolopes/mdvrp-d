#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

void** callocMatrix(int count, int pointerSize, int objectSize);
void** callocMatrix(int pointerCount, int objectCount, int pointerSize, int objectSize);
void freeMatrix(void** matrix, int pointerCount);

#endif