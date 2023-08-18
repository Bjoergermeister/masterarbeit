#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../common/C/timing.h"
#include "../../common/C/benchmark.h"

#define GRID_SIZE 1000
#define MAX_ITERATIONS 100

bool position_out_of_field(int x, int y){
    if (x < 0 || x >= GRID_SIZE) return true;
    if (y < 0 || y >= GRID_SIZE) return true;
    return false;
}

int count_neighbours(bool* field, int x, int y){
    int neighbours = 0;
    int xx, yy;
#ifdef DEBUGGING
    printf("\nChecking neighbours for [%d, %d]:\n", x, y);
#endif
    for (int dx = -1; dx < 2; dx++){
        for (int dy = -1; dy < 2; dy++){
            xx = x + dx;
            yy = y + dy;

            if (dx == 0 && dy == 0) continue;
            if (position_out_of_field(xx, yy)) continue;
#ifdef DEBUGGING
            printf("X=%d, Y=%d: %d\n", xx, yy, field[yy * GRID_SIZE + xx]);
#endif
            if (field[yy * GRID_SIZE + xx])
                neighbours++;
        }
    }
    return neighbours;
}

void calculate_next_generation(bool* source_field, bool* destination_field){
    for (int x = 0; x < GRID_SIZE; x++){
        for (int y = 0; y < GRID_SIZE; y++){
            int neighbours = count_neighbours(source_field, x, y);

            int position = y * GRID_SIZE + x;
            bool is_alive = source_field[position];
            bool will_be_alive = is_alive;
            if (is_alive && (neighbours < 2 || neighbours > 3))
                will_be_alive = false;
            else if (!is_alive && (neighbours == 3))
                will_be_alive = true;
            destination_field[position] = will_be_alive;
#ifdef DEBUGGING
            printf("[%d, %d]: Is alive: %d, neighbours: %d, will be alive: %d\n", x, y, is_alive, neighbours, will_be_alive);
#endif
        }
    }
}

#ifdef DEBUGGING
void print_field(bool* field){
    system("clear");
    char line[GRID_SIZE];
    for (int y = 0; y < GRID_SIZE; y++){
        for (int x = 0; x < GRID_SIZE; x++){
            line[x] = (field[y * GRID_SIZE + x]) ? '#' : ' ';
        }
        printf("%s\n", line);
    }
    sleep(1);
}
#endif

void randomize_field(bool* field){
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++){
        field[i] = (bool)(random() % 2);
    }
}

int main(int argc, char **argv)
{
    size_t field_size = sizeof(bool) * GRID_SIZE * GRID_SIZE;
    bool* field = (bool*)malloc(field_size);
    bool* field_copy = (bool*)malloc(field_size);

    memset(field, 0, field_size);
    memset(field_copy, 0, field_size);

    randomize_field(field);

    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    int iteration = 0;
    while(iteration < MAX_ITERATIONS){
        printf("%d\n", iteration);
#ifdef DEBUGGING
        print_field(field);
#endif
        calculate_next_generation(field, field_copy);

        // Switch fields
        dummy = field;
        field = field_copy;
        field_copy = dummy;

        iteration++;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    long difference = calculate_time_difference(&start, &end) / NANOSECONDS_IN_ONE_MICROSECOND;
    save_benchmark_result(difference, argv[1]);

    return 0;
}