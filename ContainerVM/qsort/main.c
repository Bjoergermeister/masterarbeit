#include <stdlib.h>
#include <stdio.h>

#include "../../common/C/timing.h"

#define ARRAY_SIZE 10000000

void fill_array_randomly(int* array){
    for (int i = 0; i < ARRAY_SIZE; i++){
        array[i] = random() % ARRAY_SIZE;
    }
}

int compare(const void* a, const void* b){
    return (*(int*)a - *(int*)b);
}

int main(){
    int* array = (int*)malloc(sizeof(int) * ARRAY_SIZE);
    fill_array_randomly(array);
    
    qsort(array, ARRAY_SIZE, sizeof(int), compare);

    return 0;
}