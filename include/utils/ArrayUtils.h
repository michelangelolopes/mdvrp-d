#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

void* initialize(int count, int sizeObject);
void** initialize(int count, int sizePointer, int sizeObject);
void freeMatrix(void** matrix, int count);

#endif