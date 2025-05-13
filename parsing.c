#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINES 100000

int n;
int x_vals[MAX_LINES];
int y_vals[MAX_LINES];

void read_data(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("File opening failed");
        exit(EXIT_FAILURE);
    }

    if (fscanf(fp, "%d", &n) != 1) {
        fprintf(stderr, "Invalid file format\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        if (fscanf(fp, "%d %d", &y_vals[i], &x_vals[i]) != 2) {
            fprintf(stderr, "Invalid data at line %d\n", i + 2);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    }

    fclose(fp);
}

void run(double *a, double *b) {
    long long s2 = 0, s3 = 0, s4 = 0, s5 = 0;

    for (int i = 0; i < n; i++) {
        int x = x_vals[i];
        int y = y_vals[i];

        s2 += x;
        s3 += y;
        s4 += (long long)x * x;
        s5 += (long long)x * y;
    }

    long long n0 = s4 * s3 - s2 * s5;
    long long n1 = s5 * n - s2 * s3;
    long long d  = n * s4 - s2 * s2;

    *a = (double)n0 / d;
    *b = (double)n1 / d;
}

int main() {
    read_data("preprocessed_data.txt");

    double a = 0, b = 0;

    clock_t start_clock = clock();

    for (int i = 0; i < 1000; i++) {
        run(&a, &b);
    }

    clock_t end_clock = clock();

    double cpu_time = (double)(end_clock - start_clock) / CLOCKS_PER_SEC;
    double avg_usec = (cpu_time / 1000.0) * 1e6;

    printf("Elapsed time (CPU): %.6f us per run\n", avg_usec);
    // printf("a: %.6f, b: %.6f\n", a, b);

    return 0;
}
