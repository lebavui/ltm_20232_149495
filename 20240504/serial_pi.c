#include <stdio.h>
#include <time.h>

long num_steps = 1000000000;

int main() {
    double x, sum = 0;
    double step = 1.0 / num_steps;
    
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int i = 0; i < num_steps; i++) {
        x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    clock_gettime(CLOCK_REALTIME, &stop);

    double pi = sum * step;
    printf("PI = %.10f\n", pi);

    double elapsed = (stop.tv_sec - start.tv_sec) * 1e3 + (stop.tv_nsec - start.tv_nsec) / 1e6;
    printf("Elapsed time: %f ms\n", elapsed);

    return 0;
}