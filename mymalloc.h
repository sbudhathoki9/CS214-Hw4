#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


void myinit(int allocAlg);
void* mymalloc(size_t size);
void myfree(void* ptr);
void* myrealloc(void* ptr, size_t size);
void mycleanup();
double utilization();
