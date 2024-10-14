#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

void* initialize(int count, int objectSize);
void** initialize(int count, int pointerSize, int objectSize);
void** initialize(int pointerCount, int objectCount, int pointerSize, int objectSize);
void freeMatrix(void** matrix, int pointerCount);
#endif